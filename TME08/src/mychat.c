/**
 * TP8 - EXO_2 - Mini-serveur de messagerie par communication de groupe 
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @copyright (c) 2014, toumlilt
 *
 * @version 1.0
 * @package toumlilt/M1/PR
 */


#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

int main(int argc, char** argv)
{
  /* vérification du nombre d'arguments */
  if( argc != 4 ){
    perror("Error: Usage: mychat_server <adr> <port> <pseudo>\n");
    exit(EXIT_FAILURE);
  }

  /* créatio nde la socket */
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if( sock == -1 ){
    perror("Error: socket\n");
    exit(1);
  }

  /* add multicast addr */
  struct ip_mreq imr;
  imr.imr_multiaddr.s_addr = inet_addr(argv[1]);
  imr.imr_interface.s_addr = INADDR_ANY;
  if(setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&imr
		, sizeof(struct ip_mreq)) == -1){
    perror("Error: setsockopt\n");
    exit(1);
  }

  /* init nommage */
  struct sockaddr_in sin;
  memset((char*)&sin, 0, sizeof(sin));
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(atoi(argv[2]));
  sin.sin_family = AF_INET;

  /* nommage */
  if( bind(sock, (struct sockaddr*)&sin, sizeof(sin)) == -1 ){
    perror("Error: bind\n");
    exit(1);
  }

  /* init */
  char buffer[1024];
  int i;
  
  /* deux processus : 
   * - Un sender, qui récupère les entrée sur stdin pour les envoyer
   * - Un receiver, qui reçoit les messages et les affiche
   */
  pid_t pid_son = fork();
  if( pid_son == -1 ){
    perror("Error: fork\n");
    exit(1);
  }

  if( pid_son ){ /* proc principal : sender */
    /* dest */
    struct sockaddr_in dest;
    memset((char *)&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = inet_addr(argv[1]);
    dest.sin_family = AF_INET;
    dest.sin_port = htons(atoi(argv[2]));
    
    while(1){
      
      /* lecture du message saisi par l'utilisateur */
      memset(buffer, 0, sizeof(buffer));
      strcat(buffer, argv[3]);
      strcat(buffer, ": ");
      i=strlen(buffer);
      while( (buffer[i]=getchar()) != '\n' && i < sizeof(buffer)-1 ){
	i++;
      }

      /* et on l'envoie */
      if( sendto(sock, buffer, strlen(buffer), 0,
		 (struct sockaddr*)&dest, sizeof(dest)) == -1){
	perror("Error: sendto\n");
	exit(1);
      }
    }
  }
  else { /* processus fils : receiver */
    while(1){
      memset(buffer, 0, sizeof(buffer));
      if(recvfrom(sock, buffer, sizeof(buffer), 0, NULL, NULL) == -1){
	perror("Error: recvfrom\n");
	exit(1);
      }
      printf("%s", buffer);
    }
  }
  /* fermeture de la socket */
  if( close(sock) == -1 ){
    perror("Error: close sock\n");
    exit(1);
  }
  
  return EXIT_SUCCESS;
}
