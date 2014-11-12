/**
 * TP7 - EXO_2 - Recopie de fichier par communication distante 
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define MAX_FILENAME_SIZE 64
#define MAX_BUFFER_SIZE 256

#define BACKLOG 10 /* nombre de clients dans la file ( au listen )*/

char* servDir = "./src/servDir/";

/**
 * Programme serveur principal, le récepteur.
 * Stocke les fichiers recopiés dans son répertoire d'éxecution. ( src/servDir/ )
 * Utilisation : recvfile <port>
 * @param argv[1] port le numéro de port sur lequel le récepteur attend les demandes de connexion
 */
int main(int argc, char** argv){

  /* vérification du nombre de params */
  if ( argc != 2 ){
    fprintf(stderr, "Error: Usage: recvfile <port>\n");
    return EXIT_FAILURE;
  }

  /* initialisation */
  struct addrinfo hints, *servinfo;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  hints.ai_flags = AI_PASSIVE; /* utilise mon IP */

  if( getaddrinfo(NULL, argv[1], &hints, &servinfo) != 0){
    fprintf(stderr, "Error in line __LINE__ : getaddrinfo()\n");
    exit(1);
  }

  /* loop through all the results and bind to the first we can */
  struct addrinfo *ptr;
  int sockfd;
  for(ptr = servinfo; ptr != NULL; ptr = ptr->ai_next) {
    /* création de la socket */
    if ((sockfd = socket(ptr->ai_family, ptr->ai_socktype,
			 ptr->ai_protocol)) == -1) {
      fprintf(stderr, "Error server : socket()\n");
      continue;
    }
    /* J'aime pas le "Address already in use" */
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
		   sizeof(int)) == -1) {
      fprintf(stderr, "Error server : setsockopt()\n");
      exit(1);
    }
    /* nommage du truc */
    if (bind(sockfd, ptr->ai_addr, ptr->ai_addrlen) == -1) {
      close(sockfd);
      fprintf(stderr, "Error server: bind()\n");
      continue;
    }

    break;
  }

  /* socket is in the kitchen ? */
  if( ptr == NULL){
    fprintf(stderr, "Error server : failed to connect\n");
    exit(2);
  }

  /* yes it is, plus besoin de cette structure */
  freeaddrinfo(servinfo);

  /* création de la file d'attente */
  if( listen(sockfd, BACKLOG) == -1 ){
    fprintf(stderr, "Error server : failed to listen\n");
    exit(1);
  }

  printf("Server : waiting for connections ...\n");

  /* main loop */
  while(1){
    /* buffer passe-partout */
    char buffer[MAX_BUFFER_SIZE];
    
    /* accept */
    struct sockaddr_storage client_addr; /* informations sur le sender */
    socklen_t sz = sizeof(client_addr);
    int new_fd = accept(sockfd, (struct sockaddr*)&client_addr, &sz);
    if ( new_fd == -1 ){
      fprintf(stderr, "Error server : failed to accept\n");
      exit(1);
    }

    printf("Server: got connection\n");
    
    /* réception du nom de fichier */
    int numbytes;
    char filename[MAX_FILENAME_SIZE];
    if( (numbytes = recv(new_fd, filename, MAX_FILENAME_SIZE, 0)) == -1){
      fprintf(stderr, "Error server : failed to recv filename\n");
      close(new_fd);
      close(sockfd);
      exit(1);
    }

    /* quelques vérifications sur le nom du fichier ...
     * chaîne non vide
     * ne doit pas commencer par .
     * ni contenir un /
     */
    
    /* cas nom vide */
    if( !strlen(filename) ){
      /* init du message d'erreur */
      memset(buffer, '\0', MAX_BUFFER_SIZE);
      strcat(buffer, "Error: nom du fichier vide.");
      /* envoie du message au client */
      if(send(new_fd, buffer, strlen(buffer), 0) == -1){
	perror("Error: send\n");
	exit(1);
      }
      /* fermeture de la socket */
      if( close(sockfd) == -1 ){
	fprintf(stderr, "Error : close()\n");
	return EXIT_FAILURE;
      }
    }

    /* cas du nom commençant par '.' */
    if( filename[0] == '.' ){
      /* init du message d'erreur */
      memset(buffer, '\0', MAX_BUFFER_SIZE);
      strcat(buffer, "Error: Le nom du fichier ne doit pas commencer par un '.'");
      /* envoie du message au client */
      if(send(new_fd, buffer, strlen(buffer), 0) == -1){
	perror("Error: send\n");
	exit(1);
      }
      /* fermeture de la socket */
      if( close(sockfd) == -1 ){
	fprintf(stderr, "Error : close()\n");
	return EXIT_FAILURE;
      }
      continue; 
    }

    /* cas de présence d'un '/' dans le nom */
    int i;
    for(i=0; i<numbytes; i++){
      if( filename[i] == '/' ){
	/* init du message d'erreur */
	memset(buffer, '\0', MAX_BUFFER_SIZE);
	strcat(buffer, "Error: Le nom du fichier ne doit pas contenir un '/'");
	/* envoie du message au client */
	if(send(new_fd, buffer, strlen(buffer), 0) == -1){
	  perror("Error: send\n");
	  exit(1);
	}
	/* fermeture de la socket */
	if( close(sockfd) == -1 ){
	  fprintf(stderr, "Error : close()\n");
	  return EXIT_FAILURE;
	}
	continue;
      }
    }

    /* confimation de vérification du nom de fichier */
    if(send(new_fd, "OK", 2, 0) == -1){
      perror("Error: send\n");
      exit(1);
    }

    /* ouverture du fichier */
    memset(buffer, '\0', MAX_BUFFER_SIZE);
    strcat(buffer, servDir);
    strcat(buffer, filename);
    int file_fd;
    if((file_fd=open(filename, O_RDWR | O_CREAT | O_TRUNC)) == -1){
      fprintf(stderr, "Error Server : openning filename\n");
      exit(1);
    }

    while(1){
      /*init */
      memset(buffer, '\0', MAX_BUFFER_SIZE);
      /* reception de la chaîne */
      if((numbytes=recv(new_fd, buffer, MAX_BUFFER_SIZE, 0)) == -1){
	fprintf(stderr, "Error server : failed to receive content\n");
	close(new_fd);
	exit(1);
      }
      /* si c'est la fin du fichier on quitte */
      if(!strcmp(buffer, "MYEOF")){
	break;
      }
      /* écriture dans le fichier */
      if(write(file_fd, buffer, numbytes) == -1){
	fprintf(stderr, "Error sefver : failed to write\n");
	exit(1);
      }
    }

    /* confimation de succès de transfert */
    if(send(new_fd, "OK", 2, 0) == -1){
      perror("Error: send\n");
      exit(1);
    }
    
    /* fermeture de la socket client */
    if( close(new_fd) == -1 ){
      fprintf(stderr, "Error server : failed to close client fd\n");
      exit(1);
    }

  }
		       
  /* fermeture de la socket */
  if( close(sockfd) == -1 ){
    fprintf(stderr, "Error : close()\n");
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
