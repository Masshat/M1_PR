/**
 * TP5 - EXO_3 - Mini serveur de messagerie en mémoire partagée
 * Programme Serveur
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

/* Tableau des client_id */
#define MAX_CLIENTS 256
char* clients[MAX_CLIENTS];
int nb_clients = 0;

/* ajout d'un id client dans le tableau de clients
 * le new_client_id est copié dans le tableau
 */
void add_client_id(char* new_client_id)
{
  if(nb_clients == MAX_CLIENTS){
    fprintf(stderr,"Le client %s ne peut être rajouté, tableau plein",new_client_id);
    return;
  }
  clients[nb_clients] = (char*)malloc(256*sizeof(char));
  strcpy(clients[nb_clients], new_client_id);
  nb_clients++;
}

void remove_client_id(char* client_id)
{
  int i;
  for(i=0; i<nb_clients; i++)
    if(!(strcmp(client_id, clients[i])))
      break;
  if(i==nb_clients){
    fprintf(stedrr,"error : remove_client_id, client introuvable\n");
    exit(1);
  }
  free(clients[nb_client]);
  if(;i<nb_clients; i++)
    clients[i]=clients[i+1]; /* décallage */
}
/* Principe de communication :
 * - un shm serveur, sur lequel il recevera les requêtes des clients
 * - un shm par client, sur lequel il recevera les messages broadcastés par le serv
 * - deux sémaphores par shm, pour chaque shm le diffuseur est producteur, le 
 *   récepteur est consommateur, le mutex assurera donc la consommation après
 *   production.
 */ 
char server_name[256]; /* nom du serveur */
int* sp; /* segment mappé */

char mutex_prod_name[256];
char mutex_cons_name[256];
sem_t* mutex_prod;
sem_t* mutex_cons;


/* pour la terminaison propre du programme */
void exit_setup(int exit_flag);

/* Handler des signaux reçus
 * permet de détacher et détruire proprement les shm
 * en cas de réception d'un SIGINT
 */
void sig_handler(int sig)
{
  exit_setup(1);
}

/**
 * traitement d'une requete du client
 */
void handle_request(){
  switch(sp->type){
  case REQ_TYPE_LOGIN : /* cas de demande de connexion */
    add_client_id(sp->content);
    break;
  case REQ_TYPE_LOGOUT : /* cas de demande de deconnexion */
    remove_client_id(sp->content);
    break;
  case REQ_TYPE_MSG : /* cas d'envoi de message */
    int i, ffd, *fsp;
    for(i=0; i<nb_clients; i++){
      ffd = shm_open(clients[i], O_RDRW | O_CREAT, 0600);
      *fsp = mmap(NULL, sizeof(struct request), PROT_READ | PROT_WRITE, MAP_SHARED, ffd, 0);
      fsp->content = sp->content;
      munmap(fsp, sizeof(struct request));
    }
    break;
  default :
    fprintf(stderr, "handle_request error : unkown type\n");
    exit_setup();
  }
  sem_post(request_mutex);
}

int main(int argc, char** argv)
{
  /* vérification du nombre de params */
  if ( argc != 2 ){
    fprintf(stderr, "Usage : chat_server <server_id>\n");
    return EXIT_FAILURE;
  }

  /* init nom du serveur */
  strcat(server_name, "/");
  strcat(server_name, argv[1]);
  strcat(server_name, "_shm:0");

  /* init nom sem server production */
  strcat(mutex_prod, "/");
  strcat(mutex_prod, argv[1]);
  strcat(mutex_prod, "_sem_prod");
  
  /* init nom sem server consommation */
  strcat(mutex_cons, "/");
  strcat(mutex_cons, argv[1]);
  strcat(mutex_cons, "_sem_cons");

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
  if ( (fd=shm_open(server_name, O_RDWR | O_CREAT, 0600)) == -1){
    fprintf(stderr, "error server = shm_open\n");
    exit_setup(1);
  }
  server_shm_open = 1;

  /* allocation de la taille du segment */
  if( ftruncate(fd, sizeof(struct request)) == -1){
    fprintf(stderr, "server error : ftruncate\n");
    exit_setup(1);
  }

  /* mapper le segment de mémoire */
  if( (sp=mmap(NULL, sizeof(struct request), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
    fprintf(stderr, "server error : mmap\n");
    exit_setup(1);
  }
  server_shm_mmap = 1;

  /* semaphore de communication avec le serveur */
  if((sem_server=sem_open(server_name, O_CREAT | O_EXCL | O_RDWR, 0666, 0)) == SEM_FAILED){
    fprintf(stderr,"server error : sem_open\n");
    exit_setup(1);
  }
  server_sem_server = 1;

  /* semaphore de notification serveur */
  if( (sem_notify_server=sem_open(server_notify_name, O_CREAT | O_EXCL | O_RDWR, 0666, 0)) == SEM_FAILED){
    fprintf(stderr, "server error : sem_open notify_server\n");
    exit_setup(1);
  }
  server_sem_notify_server = 1;

  /* initialisation du mutex de requete */
  if((request_mutex=sem_open("request_mutex", O_CREAT | O_EXCL | O_RDWR, 0666, 0)) == SEM_FAILED){
    fprintf(stderr,"server error : sem_open\n");
    exit_setup(1);
  } 
  server_sem_request=1;
  
  /* attente active du serveur */
  while(1){
    sem_post(sem_server);
    sem_wait(sem_notify_server);
    handle_request();/* fonction de traitement de la requete */
  }

  /* on détruit et détache le shm */
  exit_setup();
  
  return EXIT_SUCCESS;
}

/** Termine proprement le programme
 * détache et détruit le shm
 */
void exit_setup(int exit_flag)
{
  
  /* détacher le segment */
  if ( server_shm_map )
    if( munmap(sp, sizeof(struct request)) == -1){
      fprintf(stderr, "server error : munmap\n");
      exit_flag=1;
    }

  /* destruction du segment */
  if ( server_shm_open )
    if ( shm_unlink(server_name) == -1 ){
      fprintf(stderr, "server error : shm_unlink\n");
      exit_flag=1;
    }

  /* fermeture des deux sems */
  if ( sem_close(sem_server) == -1 || sem_close(request_mutex) == -1 ){
    fprintf(stderr, "server error : sem_close\n");
    exit_flag=1;
  }

  /* détachement des sems */
  if( sem_unlink(server_name) == -1 || sem_unlink("request_mutex") == -1){
    fprintf(stderr, "server_error : sem_unlink\n");
    exit_flag=1;
  }

  exit(exit_flag);
}
