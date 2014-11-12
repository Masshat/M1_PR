/**
 * TP7 - EXO_3 - Serveur d'environnement 
 * Programme client
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

#define PORTSERV 4567
char* hostname = "localhost";


int main(int argc, char** argv, char** arge)
{
  /* vérification du nombre d'arguments */
  if( argc != 1 ){
    perror("Error: Usage: sans arguments\n");
    return EXIT_FAILURE;
  }

  /* création de la socket */
  int sock;
  if( (sock=socket(AF_INET, SOCK_DGRAM, 0)) == -1){
    perror("Error: socket()\n");
    return EXIT_FAILURE;
  }

  /* init struct dest */
  struct sockaddr_in dest;
  socklen_t fromlen = sizeof(dest);
  struct addrinfo *result;
  if( getaddrinfo(hostname, 0, 0, &result) != 0){
    perror("Error: getaddrinfo\n");
    exit(1);
  }
  memset((char*)&dest, 0, sizeof(dest));
  memcpy((void*)&((struct sockaddr_in*)result->ai_addr)->sin_addr,
	 (void*)&dest.sin_addr, sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_port = htons(PORTSERV);

  /* boucle principale */
  char buffer[128];
  int i;
  while(1){
    /* demande de saisie de commande */
    printf("Veuillez saisir votre requête :\n");
    printf("S <id> <valeur>: pour un set.\n");
    printf("G <id>: pour un get.\n");
    printf("Votre requête: ");

    /* on vide le tampon ... */
    fflush(NULL);
    
    /* lecture de la commande utilisateur */
    i=0;
    memset(buffer, 0, sizeof(buffer));
    while( (buffer[i]=getchar()) != '\n' ){
      i++;
      /* pour éviter les erreurs de segmentation */
      if( i == sizeof(buffer) ){
	printf("\nWarning: commande très grande, veuillez réessayer.\n");
	printf("Votre requête: ");
	i=0;
	fflush(NULL);
      }
    }

    /* si saisie trop courte */
    if( i < 4 ){
      continue;
    }

    /* on enlève le \n de la chaîne */
    buffer[i] = '\0';

    /* envoie au serveur */
    if( sendto(sock, buffer, strlen(buffer)+1, 0,
	       (struct sockaddr*)&dest, sizeof(dest)) == -1){
      perror("Error: sendto\n");
      exit(1);
    }

    /* réception de la réponse */
    memset(buffer, 0, sizeof(buffer));
    if( recvfrom(sock, buffer, sizeof(buffer), 0, 0, &fromlen) == -1){
      perror("Error: recvfrom\n");
      exit(1);
    }

    /* affichage de la réponse */
    printf("%s\n\n", buffer);
    
  }

  /* fermeture de la socket */
  if( close(sock) == -1 ){
    perror("Error: close\n");
    exit(1);
  }
  
  return EXIT_SUCCESS;
}
