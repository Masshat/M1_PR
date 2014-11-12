/**
 * TP7 - EXO_2 - Recopie de fichier par communication distante 
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>

/**
 * Programme client principal, le sendeur.
 * Envoie d'abord le nom du fichier, puis son contenu.
 * Utilisation : sendfile <addr> <port> <filename>
 * @param argv[1] addr l'adresse à laquelle le recepteur attend les demandes de connexion
 * @param argv[2] port numéro de port recepteur
 * @param argv[3] filename le nom du fichier à recopier
 */
int main(int argc, char** argv)
{
  /* params number checking */
  if( argc != 4 ){
    fprintf(stderr, "Error, utilisation : sendfile <addr> <port> <filename>\n");
    return EXIT_FAILURE;
  }

  /* vérification de l'existance du fichier */
  if( access(argv[3], F_OK) ){
    fprintf(stderr, "Error client : %s doesn't exist\n", argv[3]);
    exit(1);
  }
  
  /* init info serv */
  struct addrinfo hints, *servinfo;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  if( getaddrinfo(argv[1], argv[2], &hints, &servinfo) != 0){
    fprintf(stderr, "Error client : getaddrinfo()\n");
    exit(1);
  }
  
  /* loop through all results and connect to the first we can */
  struct addrinfo *ptr;
  int sockfd;
  for(ptr=servinfo; ptr!=NULL; ptr=ptr->ai_next){
    /* création de la socket */
    if ((sockfd = socket(ptr->ai_family, ptr->ai_socktype,
			 ptr->ai_protocol)) == -1) {
      fprintf(stderr, "Error client : socket()\n");
      continue;
    }
    /* connexion */
    if (connect(sockfd, ptr->ai_addr, ptr->ai_addrlen) == -1) {
      close(sockfd);
      fprintf(stderr, "Error client: connect()\n");
      continue;
    }

    break; /* founded */
  }

  /* nothing found ? */
  if (ptr == NULL) {
    fprintf(stderr, "Error client: failed to connect\n");
    exit(2);
  }

  /* @TODO : plus besoin de servinfo ? */

  /* Envoi du nom de fichier */
  if(send(sockfd, argv[3], sizeof(argv[3]), 0) == -1){
    fprintf(stderr, "Error client : failed to send filename\n");
    close(sockfd);
    exit(1);
  }

  /* attente de confirmation */
  char buffer[256];
  int numbytes;
  if( (numbytes=recv(sockfd, buffer, 256, 0)) == -1){
    fprintf(stderr, "Error client : failed to recv filename confirmation\n");
    exit(1);
  }
  
  /* @TODO : traitement des erreurs */
  if(buffer[0] != 'O' || buffer[1] != 'K'){
    fprintf(stderr, "%s\n", buffer);
    exit(1);
  }
  
  /* send du contenu du fichier
   * par paquets de 256 char
   * jusqu'à EOF */

  /* ouverture du fichier */
  int file_fd = open(argv[3], O_RDONLY);
  if ( file_fd == -1 ){
    fprintf(stderr, "Error in client : open\n");
    exit(1);
  }
  /* lecture et send */
  memset(buffer, '\0', 256);
  int nb_lu;
  while((nb_lu=read(file_fd, buffer, 256)) > 0){
    if(send(sockfd, buffer, nb_lu, 0) == -1){
      fprintf(stderr, "Error client : sending file content\n");
      exit(1);
    }
    memset(buffer, '\0', 256);
  }

  /* notificaiton de fin */
  if(send(sockfd, "MYEOF", 5, 0) == -1){
    fprintf(stderr, "Error client : sending file content\n");
    exit(1);
  }
    
  /* closing the socket */
  if( close(file_fd) == -1 ){
    fprintf(stderr, "Error : close()\n");
    return EXIT_FAILURE;
  }
  
  /* closing the socket */
  if( close(sockfd) == -1 ){
    fprintf(stderr, "Error : close()\n");
    return EXIT_FAILURE;
  }

  /* mot de fin */
  printf("Le client se termine.\n");

  return EXIT_SUCCESS;
}
