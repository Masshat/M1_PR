/**
 * TP8 - EXO_3 - Sonar
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @copyright (c) 2014, toumlilt
 *
 * @version 1.0
 * @package toumlilt/M1/PR
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERV_ADDR "255.255.255.255"
#define SERV_PORT 9999

int main(int argc, char** argv)
{
  /* aucun argument */
  if( argc != 1 ){
    perror("Error: Usage: sonar ( no args ! )\n");
    return EXIT_FAILURE;
  }

  /* création de la socket */
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if( sock == -1 ){
    perror("Error: socket\n");
    exit(1);
  }

  /* add option : reuse */
  int reuse = 1;
  if( setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
		 &reuse, sizeof(reuse)) == -1){
    perror("Error: setsockopt reuse\n");
    exit(1);
  }
  if( setsockopt(sock, SOL_SOCKET, SO_REUSEPORT,
		 &reuse, sizeof(reuse)) == -1){
    perror("Error: setsockopt reuse port\n");
    exit(1);
  }

  /* add option : broadcast */
  int broadcast = 1;
  if( setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
		 &broadcast, sizeof(int)) == -1){
    perror("Error: setsockopt broadcast\n");
    exit(1);
  }

  /* init nommage */
  struct sockaddr_in sin;
  memset((char*)&sin, 0, sizeof(sin));
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(SERV_PORT);

  /* nommage */
  if( bind(sock, (struct sockaddr*)&sin, sizeof(sin)) == -1){
    perror("Error: bind\n");
    exit(1);
  }

  /* deux procs :
   * 1- pour l'attente des réponses
   * 2- pour la transmission des PING chaque 3 secondes
   */
  pid_t pid = fork();
  if( pid == -1 ){
    perror("Error: fork\n");
    exit(1);
  }
  /* Proc 1 : Attente de réponses */
  else if( pid ){
    /* init */
    char buffer[16];
    struct sockaddr_in exp;
    int fromlen = sizeof(exp);

    /* boucle d'exécution */
    while(1){
      memset(buffer, 0, sizeof(buffer));

      /* réception */
      if( recvfrom(sock, buffer, sizeof(buffer), 0,
		   (struct sockaddr*)&exp, (socklen_t*)&fromlen) == -1){
	perror("error: recvfrom\n");
	exit(1);
      }

      /* traitement */
      if( !strncmp(buffer, "PONG", 4)){
	printf("PONG : <IP = %s, PORT = %d>\n",
	       inet_ntoa(exp.sin_addr),
	       ntohs(exp.sin_port));
      }
    }
  }
  /* Proc 2 : PING chaque 3 sec */
  else {
    /* init */
    char* ping = "PING";
    struct sockaddr_in dest;
    dest.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    dest.sin_family = AF_INET;
    dest.sin_port = htons(SERV_PORT);
    
    /* boucle d'exécution */
    while(1){
      /* send */
      if( sendto(sock, ping, sizeof(ping), 0,
		 (struct sockaddr*)&dest, sizeof(dest)) == -1){
	perror("Error: sendto\n");
	exit(1);
      }

      /* notif */
      printf("sent PING\n");

      sleep(3);
    }
  }
  

  /* fermeture de la socket */
  if( close(sock) == -1){
    perror("Error: close\n");
    exit(2);
  }
  
  return EXIT_SUCCESS;
}
