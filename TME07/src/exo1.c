/**
 * TP7 - EXO_1 - remontée de valeurs par communications distantes
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @copyright (c) 2014, toumlilt
 *
 * @version 1.0
 * @package toumlilt/M1/PR
 */

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define NB_FILS 5
#define PORTSERV 4567
#define MSG_SIZE 128

char *hostname = "localhost";

/**
 * Fonction exec par les fils ( les clients ),
 * génération de la random_val qui sera écrite dans le fichier
 */
void fx_fils(int index)
{
  /* génération de la random_val */
  srand(getpid());
  int random_val = (int) ((int)rand() % 10);
  printf("Fils %d : generated random_val=%d\n", index, random_val);
  
  /* création */
  int scc;
  if( (scc = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
    fprintf(stderr, "Error in fx_fils() : socket()\n");
    exit(1);
  }

  /* initialisation */
  int err;
  struct addrinfo hints, *result;
  struct sockaddr_in s_recep;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = 0;
  if( ( err = getaddrinfo(hostname, 0, &hints, &result)) != 0){
    fprintf(stderr, "Error in fx_fils() : getaddrinfo() error=%d\n", err);
    exit(1);
  }
  memset((char *)&s_recep, 0, sizeof(s_recep));
  memcpy((void*)&((struct sockaddr_in*)result->ai_addr)->sin_addr, (void*)&s_recep.sin_addr, sizeof(s_recep));
  s_recep.sin_family = AF_INET;
  s_recep.sin_port = htons(PORTSERV);

  /* construction du mmessage */
  char message[MSG_SIZE];
  memset(message, '\0', MSG_SIZE);
  sprintf(message, "%d", random_val);

  /* Envoie du message */
  if( sendto(scc, (void *)message, strlen(message), 0, (struct sockaddr *)&s_recep, sizeof(s_recep)) == -1){
    fprintf(stderr, "Error in fx_fils() : sendto()\n");
    exit(1);
  }

  /* fermeture de la socket */
  close(scc);
}

int main(int argc, char** argv)
{
  /* création des fils */
  int i, pid_fils;
  for(i=0; i<NB_FILS; i++){
    if( (pid_fils = fork()) == -1){
      fprintf(stderr, "Error in main() : fork()\n");
      return EXIT_FAILURE;
    } else if( pid_fils == 0){
      fx_fils(i); /* le fils fait ce qu'il a à faire */
      exit(0); /* puis se termine */
    }
  }

  /* PARTIE SERVEUR */

  /* création de la socket */
  int sc;
  if( (sc = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    fprintf(stderr, "Error in main() : socket()\n");
    exit(1);
  }

  /* initialisation */
  struct sockaddr_in s_serv; /* nom de la socket du serveur */
  memset((char*)&s_serv, 0, sizeof(s_serv));
  s_serv.sin_addr.s_addr = htonl(INADDR_ANY);
  s_serv.sin_port = htons(PORTSERV);
  s_serv.sin_family = AF_INET;

  /* nommage de la socket */
  if(bind(sc, (struct sockaddr*)&s_serv, sizeof(s_serv)) == -1){
    fprintf(stderr, "Error in main() : bind()\n");
    exit(1);
  }

  /* réception des random_val + calcul de la somme*/
  char message[MSG_SIZE];
  struct sockaddr_in s_recep;
  socklen_t s_recep_len = sizeof(s_recep);
  int somme = 0, lu;
  for( i=0; i<NB_FILS; i++){
    if( recvfrom(sc, message, sizeof(message), 0, (struct sockaddr*)&s_recep, &s_recep_len) == -1){
      fprintf(stderr, "Error in main() : recvfrom()\n");
      exit(1);
    }
    sscanf(message, "%d", &lu);
    printf("PAPA : received %d\n", lu);
    somme += lu;
  }

  /* fermeture */
  close(sc);

  /* attente et vérification de bonne terminaison des fils */
  int status;
  for(i=0; i<NB_FILS; i++){
    wait(&status);
    if( !WIFEXITED(status) || WEXITSTATUS(status) ){
      fprintf(stderr, "Error in main() : un fils s'est mal terminé\n");
      return EXIT_FAILURE;
    }
  }

  /* affichage de la somme */
  printf("Main : somme des valeurs aléatoires = %d\n", somme);

  return EXIT_SUCCESS;
}
