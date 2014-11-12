/**
 * TP7 - EXO_3 - Serveur d'environnement 
 * Programme serveur
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

/*******************************************************************************
 * Private functions :
 ******************************************************************************/
void set(char* id, char* value);
char* get(char* id);

/*******************************************************************************
 * Implementation :
 ******************************************************************************/
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
    exit(1);
  }

  /* nommage */
  struct sockaddr_in sin;
  memset((char*)&sin, 0, sizeof(sin));
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(PORTSERV);
  sin.sin_family = AF_INET;
  if( bind(sock, (struct sockaddr*)&sin, sizeof(sin)) == -1){
    perror("Error: bind()\n");
    exit(1);
  }

  /*boucle principale */
  char buffer[128], tmp[64];
  char id[64];
  char value[64];
  struct sockaddr_in exp; /* expediteur */
  socklen_t fromlen = sizeof(exp);
  
  while(1){
    /* init */
    memset(buffer, '\0', sizeof(buffer));
    memset(id, '\0', 64);
    memset(value, '\0', 64);
    
    /* réception */
    if( recvfrom(sock, buffer, sizeof(buffer), 0,
		 (struct sockaddr*)&exp, &fromlen) == -1){
      perror("Error: recvfrom\n");
      exit(1);
    }


    /* traitement */
    if( buffer[0] == 'S' ){
      if( sscanf(buffer, "%s %s %s", tmp, id, value) == 3){
	set(id, value);
	memset(buffer, '\0', sizeof(buffer));
	strcat(buffer, "set: OK");
      } else {
	memset(buffer, '\0', sizeof(buffer));
	strcat(buffer, "set: KO: format incorrect");
      }
    }
    else if( buffer[0] == 'G' ){
      if( sscanf(buffer, "%s %s", tmp, id) == 2){
	memset(buffer, '\0', sizeof(buffer));
	strcat(buffer, "get: ");
	char *res = NULL;
	if( (res = get(id)) ){
	  strcpy(value, res);
	  strcat(buffer, value);
	} else {
	  strcat(buffer, "Unknown Id");
	}
      } else {
	memset(buffer, '\0', sizeof(buffer));
	strcat(buffer, "get: KO: format invalide");
      }
    }
    else {
      memset(buffer, '\0', sizeof(buffer));
      strcat(buffer, "unknown request");
    }

    /* retour de réponse */
    if( sendto(sock, (void*)buffer, sizeof(buffer), 0,
	       (struct sockaddr*)&exp, fromlen) == -1){
      perror("Error: sendto\n");
      exit(1);
    }

  }
  
  /* fermeture de la socket */
  if( close(sock) == -1 ){
    perror("Error: close()\n");
    exit(1);
  }
  
  return EXIT_SUCCESS;
}

void set(char* id, char* value)
{
  setenv(id, value, 1);
}

char* get(char* id)
{
  return getenv(id);
}
