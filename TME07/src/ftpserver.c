/**
 * TP7 - EXO4 - Mini-serveur FTP 
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
#include <sys/stat.h>
#include <fcntl.h>

#define BACKLOG 1 /* nb of listening clients in the queue */
#define BUF_SIZE 128

/*******************************************************************************
 * Private declarations :
 ******************************************************************************/
void *get_in_addr(struct sockaddr *sa);
int check_filename(char* filename, int size);
void exec_ftp_upload_request(int new_fd, char* dir_path);


/*******************************************************************************
 * Implementations :
 ******************************************************************************/
/**
 * Programme serveur principal, le serveur FTP.
 * Utilisation : ftpserver <port> <dir_path>
 * @param argv[1] port sur lequel le serveur attend les connexions
 * @param argv[2] dir_path le chemin vers le rép de sauvegarde des fichiers uploadés
 * le serveur effectue trois types d'opérations :
 * UPLOAD : téléchargement de fichier depuis le client vers le serveur.
 * DOWNLOAD : téléchargement de fichier depuis le serveur vers le client.
 * LIST : obtention de la liste des fichiers conservés sur le serveur.
 * ! On considère que le serveur ne peut gérer qu'un seul client à la fois.
 */
int main(int argc, char** argv)
{
  
  /* checking the number of args */
  if( argc != 3){
    fprintf(stderr, "Error, Usage : ftpserver <port> <dir_path>\n");
    return EXIT_FAILURE;
  }

  /* renaming args */
  char* port = argv[1];
  char* dir_path = argv[2];

  /* si dir_path n'existe pas encore, il sera créé */
  mkdir(dir_path, 0755);

  /* inits */
  struct addrinfo hints, *servinfo;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  hints.ai_flags = AI_PASSIVE; /* use my IP */
  
  if(getaddrinfo(NULL, port, &hints, &servinfo) != 0){
    fprintf(stderr, "Error : getaddrinfo() failed\n");
    exit(1);
  }

  /* loop through all the results and bind to the first we can */
  struct addrinfo *ptr;
  int sockfd;
  for(ptr = servinfo; ptr != NULL; ptr = ptr->ai_next) {
    /* creating the socket */
    if((sockfd = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol)) == -1) {
      fprintf(stderr, "Error server : socket()\n");
      continue;
    }
    /* I don't like the "Address already in use" */
    int yes = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
		  sizeof(int)) == -1) {
      fprintf(stderr, "Error server : setsockopt()\n");
      exit(1);
    }
    /* namming that thing */
    if(bind(sockfd, ptr->ai_addr, ptr->ai_addrlen) == -1) {
      close(sockfd);
      fprintf(stderr, "Error server : bind()\n");
      continue;
    }
    break;
  }

  /* nothing found ? */
  if(ptr == NULL){
    fprintf(stderr, "Error server : failed to connect\n");
    exit(2);
  }

  /* we don't need this anymore */
  freeaddrinfo(servinfo);
  
  /* creating the waiting queue */
  if( listen(sockfd, BACKLOG) == -1){
    fprintf(stderr, "Error server : failed to listen()\n");
    exit(1);
  }

  printf("Server : waiting for connections ...\n");

  /* accept */
    struct sockaddr_storage client_addr; /* informations sur le sender */
    socklen_t sz = sizeof(client_addr);
    int new_fd = accept(sockfd, (struct sockaddr*)&client_addr, &sz);
    if ( new_fd == -1 ){
      fprintf(stderr, "Error server : failed to accept\n");
      exit(1);
    }

    /* confirmation de connection */
    char s[20];
    inet_ntop(client_addr.ss_family,
	      get_in_addr((struct sockaddr *)&client_addr),
	      s, sizeof s);
    printf("Server: got connection from %s\n", s);
    
  /* main loop */
  while(1){
    printf("Server: waiting for new request ...\n");

    /* réception des requetes */
    int numbytes;
    char type;
    if( (numbytes = recv(new_fd, &type, sizeof(char), 0)) == -1){
      perror("Error : recv request type\n");
      exit(1);
    }

    if( type == 'U' ){
      exec_ftp_upload_request(new_fd, dir_path);
    }
    
    /* fermeture */
    if( close(new_fd) == -1 ){
      fprintf(stderr, "Error server : failed to close client fd\n");
      exit(1);
    }
  }
  
  /* closing the socket */
  if( close(sockfd) == -1){
    fprintf(stderr, "Error : close()\n");
    exit(1);
  }

  return EXIT_SUCCESS;
}

/**
 * get sockaddr, IPv4 or IPv6:
 */
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/**
 * vérifie la validité d'un nom de fichier
 * contraintes :
 * - ne doit pas être une chaine vide
 * - ne doit pas contenir de "/" 
 * - ne doit pas débuter avec "."
 * @param filename la chaine a traiter
 * @return 1 si correcte, 0 sinon.
 */
int check_filename(char* filename, int size)
{
  if(!size)
    return 0;
  
  if(filename[0] == '.')
    return 0;

  int i=1;
  for(; i<size; i++)
    if(filename[i]=='/')
      return 0;

  return 1;
}

void exec_ftp_upload_request(int new_fd, char* dir_path)
{
  char buffer[BUF_SIZE];
  char c;
  int i;
  
  /* init filename */
  char filename[65];
  i = 0;
  memset(buffer, 0, sizeof(buffer));
  memset(filename, 0, sizeof(filename));

  while( recv(new_fd, &c, sizeof(char), 0) > 0){
    buffer[i] = c;
    if( c == '\0' )
      break;
    i++;
  }
  
  if( !check_filename(buffer, i) ){
    close(new_fd);
    return ;
  }
  
  strcat(filename, dir_path);
  if( filename[strlen(filename)-1] != '/' )
    strcat(filename, "/");
  strncat(filename, buffer, i);
  
  /* ouverture du fichier */
  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC , 0600);
  if( fd == -1 ){
    perror("Error: upload_func: open\n");
    exit(1);
  }

  /* réception de la taille du fichier */
  int filesize;
  if( recv(new_fd, &filesize, sizeof(int), 0) == -1 ){
    perror("Error: upload_func: recv filesize\n");
    exit(1);
  }
  filesize = ntohl(filesize);

  /* réception du contenu du fichier + écriture */
  for(i=0; i<filesize; i++){
    if( recv(new_fd, &c, sizeof(char), 0) == -1){
      perror("Error: upload_func: recv file content\n");
      exit(1);
    }
    if( write(fd, &c, sizeof(char)) == -1){
      perror("Error: upload_func: write file content\n");
      exit(1);
    }
  }
  
  if( close(fd) == -1 ){
    perror("Error: upload: close\n");
    exit(1);
  }

  printf("UPLOAD: done !\n");
}


