/**
 * TP5 - EXO_3 - Mini serveur de messagerie en mémoire partagée
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
#include <sys/mman.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>

#include <chat.h>

char* client_name[256] = "/"; /* nom du client */
char* client_notify_name[256] = "/no"; 
char* server_name[256] = "/"; /* nom du serveur */
char* server_notify_name[256] = "/no"; /* nom du sem de notification serveur */

int *sp; /* map du shm */

sem_t* sem_server; /* sémaphore de communication avec le serveur */
sem_t* sem_request; /* mutex sur la request */
sem_t* sem_notify_request; /* sem de notification client */
sem_t* sem_notify_server; /* sem de notification serveur */

/* définition des flags pour la terminaison propre du programme
 * chaque flag indique si un shm/sem a été créé pour gérer sa terminaison propre
 * 0: pas encore créé
 * 1: créé
 */
char client_connected = 0;
char client_shm_open = 0;
char client_shm_mmap = 0;
char client_sem_server = 0;
char client_sem_request = 0;
char client_sem_notify_server = 0;
char client_sem_notify_request = 0;
void exit_setup(int exit_flag); /* fonction qui gère la terminaison propre du programme ( définition ci-dessous ) */

int main(int argc, char** argv)
{
  /* vérification du nombre d'arguments */
  if( argc != 3 ){
    fprintf(stderr, "error : Le programme prend 2 params <client_id> et <server_id>\n");
    return EXIT_FAILURE;
  }
  
  /* initialisation du nom du client */
  strcat(client_name,argv[1]);
  strcat(client_name,"_shm:0");

  /* initialisation du nom du serveur */
  strcat(server_name,argv[2]);
  strcat(server_name,"_shm:0");

  /* initialisation du notify server */
  strcat(server_notify_name,argv[2]);

  /* initialisation du notify client */
  strcat(client_notify_name,argv[1]);

  /* installation du handler de signaux
   * va servir à détacher et détruire proprement 
   * le shm à la réception d'un SIGINT
   */
  sigset_t set; /* masque des signaux */
  struct sigaction act; /* traitement des signaux */
  sigemptyset(&set);
  sigaddset(&set, SIGINT);
  act.sa_mask = set;
  act.sa_flags=0;
  act.sa_handler=sig_handler;
  sigaction(SIGINT, &act, NULL);

  /* création du shm */
  int fd;
  if( (fd=shm_open(client_name, O_RDWR | O_CREAT, 0600)) == -1){
    fprintf(stderr, "error client %s : shm_open\n",argv[1]);
    exit_setup(1);
  }
  client_shm_open=1;

  /* allocation de la taille du segment */
  if( ftruncate(fd, sizeof(struct request)) == -1){
    fprintf(stderr, "error client %s : shm_open\n",argv[1]);
    exit_setup(1);
  }

  /* mapper le segment mémoire */
  if( (sp=mmap(NULL, sizeof(struct request), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
    fprintf(stderr, "error client %s : mmap\n", argv[1]);
    exit_setup(1);
  }
  client_shm_map=1;
  
  /* sémaphore de communication avec le serveur */
  if ( (sem_server=sem_open(server_name, O_CREAT | O_EXCL | O_RDWR, 0666, 0)) == SEM_FAILED ){
    fprintf(stderr, "error client %s : sem_open server\n", argv[1]);
    exit_setup(1);
  }
  client_sem_server=1;

  /* sémaphore de notification serveur */
  if( (sem_notify_server=sem_open(server_notify_name, O_CREAT | O_EXCL | O_RDWR, 0666, 0)) == SEM_FAILED ){
    fprintf(stderr, "error client %s : sem_open server_notify\n", client_name);
    exit_setup(1);
  }

  /* sémaphore sur la request */
  if( (sem_request=sem_open(client_name, O_CREAT | O_EXCL | O_RDWR, 0666, 0)) == SEM_FAILED ){
    fprintf(stderr, "error client %s : sem_open server\n", argv[1]);
    exit_setup(1);
  }
  client_sem_request=1;
  
  /* sémaphore sur la notif de la requete */
  if( (sem_notify_request=sem_open(client_notify_name, O_CREAT | O_EXCL | ORDWR, 0666, 0)) == SEM_FAILED ){
    fprintf(stderr, "error client %s : sem_open notify_request\n", client_name);
    exit_setup(1);
  }

  /* connexion au serveur */
  sem_wait(sem_server);
  struct request connect;
  connect.type = 0;
  connect.content = client_name;
  sp = &connect;
  sem_post(sem_notify_server);
  client_connected=1;

  /* attente active sur entrée de message ou réception */
  char buffer[1024];
  int sem_val;
  while(1){
    read(STDIN_FILENO, buffer, 1024);
    sem_getvalue(sem_notify_request, &sem_val);
    if ( sem_val > 0 ){
      sem_wait(sem_notify_request);
      printf("Message : %s\n", sp->content);
      sem_post(sem_request);
    }  
  }

  /* on termine proprement */
  exit_setup(0);

  return EXIT_SUCCESS;
}

/**
 * terminaison propre du programme
 * suppression et destruction des shm/sem avant terminaison
 * exit_flag : etat de la terminaison, 0=correcte, 1:avec erreur
 */
void exit_setup(int exit_flag)
{
  /* déconnexion */
  if ( client_connected ){
    struct request logout;
    logout.type = 2;
    logout.content = client_name;
    sem_wait(sem_server);
    *sp = logout;
    sem_post(sem_notify_server);
  }

  /* détacher le segment client */
  if ( client_shm_mmap )
    if( munmap(sp, sizeof(struct request)) == -1){
      fprintf(stderr, "error client %s : munmap\n", client_name);
      exit_flag=1;
    }

  /* destruction du segment */
  if ( client_shm_open )
    if( shm_unlink(client_name) == -1 ){
      fprintf(stderr, "error client %s : shm_unlink\n", client_name);
      exit_flag=1;
    }

  /* fermeture du sem serveur */
  if ( client_sem_server )
    if( sem_close(sem_server) == -1 ){
      fprintf(stderr, "error client %s : sem_close\n", client_name);
      exit_flag=1;
    }

  /* fermeture du sem notification serveur */
  if ( client_sem_notify_server )
    if( sem_close(sem_notify_server) == -1){
      fprintf(stderr, "error client %s : sem_close\n", client_name);
      exit_flag=1;
    }

  /*fermeture du sem request */
  if ( client_sem_request )
    if( sem_close(sem_request) == -1 ){
      fprintf(stderr, "error client %s : sem_close_request\n", client_name);
      exit_flag=1;
    }

  /* détachement du sem request */
  if ( client_sem_request )
    if ( sem_unlink(client_name) == -1){
      fprintf(stderr, "error client %s : sem_unlink request\n", client_name);
      exit_flag=1;
    }

  /* fermeture du sem norify_request */
  if( client_sem_notify_request )
    if( sem_close(sem_notify_request) == -1 ){
      fprintf(stderr, "error client %s : sem_notify_request\n", client_name);
      exit_flag=1;
    }

  /* détachement du sem request */
  if( client_sem_notify_request )
    if( sem_unlink(client_notify_name) == -1 ){
      fprintf(stderr, "error client %s : sem_unlink\n", client_name);
      exit_flag=1;
    }

  exit(exit_flag);
}
