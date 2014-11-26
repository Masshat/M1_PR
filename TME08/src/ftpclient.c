/**
 * TP7 - EXO_4 - Mini serveur FTP
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
#include <sys/stat.h>

#define STDIN_BUF_SIZE 256

void exec_ftp_request(char* request, int sockfd);
void exec_ftp_upload_request(char* filename, int sockfd);

/**
 * Programme client principal, le client FTP.
 * Utilisation : ftpclient <addr> <port>
 * @param argv[1] addr l'adresse à laquelle le serveur attend les demandes de connexion
 * @param argv[2] port numéro de port récepteur
 * Une fois connecté au serveur, un client peut requérir chacune des opérations
 * en envoyant un message contenant la chaîne de caractères correspondante
 */
int main(int argc, char** argv)
{
  
  /* chacking number of args */
  if( argc != 3 ){
    fprintf(stderr, "Error, Usage : ftpclient <addr> <port>\n");
    return EXIT_FAILURE;
  }

  /* renaming args */
  char* addr = argv[1];
  char* port = argv[2];

  /* init info serv */
  struct addrinfo hints, *servinfo;
  int rv;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  if( (rv = getaddrinfo(addr, port, &hints, &servinfo)) != 0){
    fprintf(stderr, "Error client : getaddrinfo : %s\n", gai_strerror(rv));
    exit(1);
  }
  
  /* loop through all results and connect to the first we can */
  struct addrinfo *ptr;
  int sockfd;
  for(ptr=servinfo; ptr !=NULL; ptr=ptr->ai_next){
    /* creation the socket */
    if((sockfd=socket(ptr->ai_family, ptr->ai_socktype,
		      ptr->ai_protocol)) == -1){
      fprintf(stderr, "Error client : socket()\n");
      continue;
    }
    /* connect */
    if(connect(sockfd, ptr->ai_addr, ptr->ai_addrlen) == -1){
      close(sockfd);
      fprintf(stderr, "Error client : connect()\n");
      continue;
    }
    break; /* founded */
  }

  /* nothing found ? */
  if(ptr == NULL){
    fprintf(stderr, "Error client : failed to connect\n");
    exit(2);
  }

  /* plus besoin de servinfo */
  freeaddrinfo(servinfo);

  /* confirmation de connexion */
  printf("Connexion réussie !\n");

  /*init */
  char request[STDIN_BUF_SIZE];
  
  /* boucle principale */
  while(1){

    /* invite de commande */
    printf("Que puis-je faire pour vous ?\n");
    printf("> UPLOAD: envoyer filename\n");
    printf("> DOWNLOAD: télécharger filename\n");
    printf("> LIST: obtention de la liste des fichiers\n");
    printf("> ");
    fflush(NULL);
    
    /* récupération stdin */
    int i=0;
    memset(request, 0, STDIN_BUF_SIZE);
    while( (request[i]=getchar()) != '\n'){
      i++;
      /* pour éviter les erreurs de segmentation */
      if( i == STDIN_BUF_SIZE ){
	printf("\nWarning: commande très grande, veuillez réessayer.\n");
	printf("Votre requête: ");
	i=0;
	fflush(NULL);
      }
    }

    /* si saisie trop courte */
    if( i < 5 ){
      continue;
    }

    /* on enlève le \n de la chaîne lue */
    request[i] = '\0';

    /* execution de la requête lue */
    exec_ftp_request(request, sockfd);
  }
  
  if( close(sockfd) == -1){
    fprintf(stderr, "Error : close()\n");
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}

void exec_ftp_request(char* request, int sockfd)
{
  /* cas UPLOAD */
  if( !strncmp("UPLOAD", request, 6) ){
    exec_ftp_upload_request(request+7, sockfd);
  }
  else {
    printf("Unknown request !\n");
  }
}

void exec_ftp_upload_request(char* filename, int sockfd)
{
  /* check existance of filename */
  if( access(filename, F_OK) ){
    printf("Error : %s does not exist\n", filename);
    return ;
  }

  /* send request, U for UPLOAD */
  if( send(sockfd, "U", sizeof(char), 0) == -1){
    perror("Error : send UPLOAD\n");
    exit(1);
  }
  
  /* send filename */
  int i;
  for(i=0; i<=strlen(filename); i++){
    if( send(sockfd, filename+i, sizeof(char), 0) == -1){
      perror("Error : send filename\n");
      exit(1);
    }
  }

  /* open file */
  int fd;
  if( (fd = open(filename, O_RDONLY)) == -1 ){
    perror("Error : read filename\n");
    exit(1);
  }

  /* récupération de la taille du fichier */
  struct stat buf;
  if( fstat(fd, &buf) == -1){
    perror("Error: upload_func: fstat\n");
    exit(1);
  }
  int filesize = htonl(buf.st_size);

  /* envoie de la taille du fichier */
  if( send(sockfd, &filesize, sizeof(int), 0) == -1 ){
    perror("Error: upload_func: send filesize\n");
    exit(1);
  }

  /* lecture et envoie du contenu du fichier */
  char c;
  int nblu;
  while( (nblu = read(fd, &c, sizeof(char))) ){
    if( nblu == -1 ){
      perror("Error: upload_func: read file content\n");
      exit(1);
    }
    
    if( send(sockfd, &c, sizeof(char), 0) == -1){
      perror("Error: upload_func: send file content\n");
      exit(1);
    }
  }

  /* close file */
  if( close(fd) == -1){
    perror("Error: ftp_upoad: close\n");
    exit(1);
  }

  printf("Fichier envoyé avec succès\n");
}
