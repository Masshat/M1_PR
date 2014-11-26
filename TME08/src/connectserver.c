/**
 * TP8 - EXO_4 - Journalisation de connexions
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @copyright (c) 2014, toumlilt
 *
 * @version 1.0
 * @package toumlilt/M1/PR
 */

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

/* log file path */
char* cxlog_file = "src/servDir/cxlog";

int main(int argc, char** argv)
{
  /* au moins un arg doit exister */
  if( argc < 2 ){
    fprintf(stderr, "Error: Au moins un argument doit être transmis\n");
    fprintf(stderr, "Usage: cxloger <port1> [<port2> ...]\n");
    return EXIT_FAILURE;
  }

  /* init ports array */
  int ports[argc-1];
  int i;
  for(i=0; i<argc-1; i++){
    ports[i] = atoi(argv[i+1]);
  }

  /* ouverture du fichier des logs */
  int fd = open(cxlog_file, O_WRONLY | O_TRUNC | O_CREAT, 0666);
  if( fd == -1 ){
    perror("Error: open log file\n");
    exit(1);
  }

  /* init */
  int sock_arr[argc-1];
  struct sockaddr_in sin;

  /* boucle d'init par port */
  for(i=0; i<argc-1; i++){
    /* création de la socket */
    sock_arr[i] = socket(AF_INET, SOCK_STREAM, 0);
    if( sock_arr[i] == -1 ){
      perror("Error: socket\n");
      exit(i+1);
    }
    
    /* init sin struct */
    memset((char*)&sin, 0 ,sizeof(sin));
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(ports[i]);
    sin.sin_family = AF_INET;

    /* nommage */
    if( bind(sock_arr[i], (struct sockaddr*)&sin, sizeof(sin)) == -1){
      perror("Error: bind\n");
      exit(i+1);
    }

    /* écoute */
    if( listen(sock_arr[i], 10) == -1){
      perror("Error: listen\n");
      exit(i+1);
    }
  }

  /* init */
  fd_set ens;
  FD_ZERO(&ens);
  for(i=0; i<argc-1; i++)
    FD_SET(sock_arr[i], &ens);

  struct sockaddr_in exp;
  int fromlen = sizeof(exp);

  /* boucle d'exécution */
  while(1){
    /* attente */
    if( select(sock_arr[argc-2]+1, &ens, NULL, NULL, NULL) == -1 ){
      perror("Error: select\n");
      exit(1);
    }

    /* traitement */
    for(i=0; i<argc-1; i++){
      if( FD_ISSET(sock_arr[i], &ens) ){
	/* on accepte la connexion */
	if( accept(sock_arr[i], (struct sockaddr*)&exp,
		   (socklen_t*)&fromlen) == -1) {
	  perror("Error: accept\n");
	  exit(i+1);
	}
	/* confirm */
	printf("got client : <IP = %s, PORT = %d>\n",
	       inet_ntoa(exp.sin_addr),
	       ntohs(exp.sin_port));
	/* et on écrit l'IP du client dans le log file */
	if( write(fd, inet_ntoa(exp.sin_addr),strlen(inet_ntoa(exp.sin_addr)))
	    == -1 ){
	  perror("Error: write\n");
	  exit(i+1);
	}
	/* retour à la ligne ... */
	if( write(fd, "\n", sizeof(char)) == -1 ){
	  perror("Error: write return\n");
	  exit(i+1);
	}
	fflush(NULL);
      }
    }
  }

  /* fermeture des sockets */
  for(i=0; i<argc-1; i++){
    if( close(sock_arr[i]) == -1){
      fprintf(stderr, "Error: close sock num%d\n", i);
      exit(1);
    }
  }

  /* fermeture log file */
  if( close(fd) == -1 ){
    perror("Error : close log file\n");
    exit(1);
  }
    
  return EXIT_SUCCESS;
}
