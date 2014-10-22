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

/* Principe de communication :
 * - un shm serveur, sur lequel il recevera les requêtes des clients
 * - un shm par client, sur lequel il recevera les messages broadcastés par le serv
 * - deux sémaphores par shm, pour chaque shm le diffuseur est producteur, le 
 *   récepteur est consommateur, le mutex assurera donc la consommation après
 *   production.
 */ 
char server_name[256]; /* nom du serveur */
int* sp_shm_server; /* segment mappé */

char mutex_prod_name[256];
char mutex_cons_name[256];
sem_t* mutex_prod;
sem_t* mutex_cons;

/* données clients : */
#define MAX_CLIENTS 10
char* clients_ids[MAX_CLIENTS];
int nb_connected=0;

sem_t* mutex_prod_client[MAX_CLIENTS]; 
sem_t* mutex_cons_client[MAX_CLIENTS];

/*******************************************************************************
 * Fonction privées ( prototypes )
 ******************************************************************************/
void exit_setup(int exit_value);
void sigint_handler(int signum);
void handle_request();
/*******************************************************************************
 * Implementation
 ******************************************************************************/
int main(int argc, char** argv)
{
  /* vérification du nombre de params */
  if ( argc != 2 ){
    fprintf(stderr, "Usage : chat_server <server_id>\n");
    return EXIT_FAILURE;
  }

  /* installation du handler de SIGINT
   * va servir à détacher et détruire proprement
   * tout ce qu'il y'a à nettoyer avant la terminaison 
   * lance exit_setup() ...
   */
  sigset_t set; /* masque des signaux */
  struct sigaction act; /* traitement des signaux */
  sigemptyset(&set);
  sigaddset(&set, SIGINT);
  act.sa_mask = set;
  act.sa_flags=0;
  act.sa_handler=sigint_handler;
  sigaction(SIGINT, &act, NULL);
  
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

  /* création du shm server */
  int fd_shm_server;
  if ( (fd_shm_server=shm_open(server_name, O_RDWR | O_CREAT, 0600)) == -1){
    fprintf(stderr, "error server = shm_open\n");
    exit_setup(1);
  }

  /* allocation de la taille du segment */
  if( ftruncate(fd_shm_server, sizeof(struct request)) == -1){
    fprintf(stderr, "server error : ftruncate\n");
    exit_setup(1);
  }

  /* map du shm server */
  if( (sp_shm_server=mmap(NULL, sizeof(struct request), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm_server, 0)) == MAP_FAILED){
    fprintf(stderr, "server error : mmap\n");
    exit_setup(1);
  }

  /* création du semaphore de production sur le shm server */
  if( (mutex_prod=sem_open(mutex_prod_name, O_CREAT | 0_EXCL | O_RDWR, 0666, 0))
      == SEM_FAILED){
    fprintf(stderr, "Server error: sem_open: mutex_prod\n");
    exit_setup(1);
  }

  /* initialisation du sem prod sur shm server */
  if( sem_init(mutex_prod, 1, 1) == -1){
    fprintf(stderr, "Server error: sem_init: mutex_prod\n");
    exit_setup(1);
  }

  /* création du sem de consommation sur le shm server */
  if( (mutex_cons=sem_open(mutex_cons_name, O_CREAT | 0_EXCL | O_RDWR, 0666, 0))
      == SEM_FAILED){
    fprintf(stderr, "Server error: sem_open: mutex_cons\n");
    exit_setup(1);
  }

  /* initialisation du sem prod sur shm server */
  if( sem_init(mutex_cons, 1, 0) == -1){
    fprintf(stderr, "Server error: sem_init: mutex_cons\n");
    exit_setup(1);
  }

  /* le corps du programme, enfin ! */
  while(1){
    /* on attend l'arrivée d'une requête */
    sem_wait(mutex_cons);

    /* traitement de la requête */
    handle_request();
    
    /* libération de production */
    sem_post(mutex_prod);
  }
  
  /* terminaison propre */
  exit_setup(EXIT_SUCCESS);
  
  return EXIT_SUCCESS;
}

/**
 * Terminaison propre avec tous les nettoyages et désallocs ...
 * il se peut qu'on fasse appel à cette fonction alors qu'une ou plusieurs
 * parties n'ont pas encore étaient créée donc on va ignorer les erreurs des 
 * appels ...
 */
void exit_setup(int exit_value)
{
  /* détachement du shm server */
  munmap(sp_shm_server, sizeof(struct request));

  /* destruction du shm server */
  shm_unlink(server_name);

  /* fermeture du sem prod sur shm server */
  sem_close(mutex_prod);

  /* destruction du sem prod sur shm server */
  sem_unlink(mutex_prod_name);

  /* fermeture du sem cons sur shm server */
  sem_close(mutex_cons);

  /* destruction du sem cons sur shm server */
  sem_unlink(mutex_cons_name);
  

  exit(exit_value);  
}

void sigint_handler(int signum)
{
  printf("SIGINT reçu: clean exit ...\n");
  exit_setup(1);
}

/**
 * Traite une requête du client
 */
void handle_request()
{
  switch(sp_shm_server->type){
  case REQ_TYPE_LOGIN : /* cas d'une demande de connexion */
    break;
  case REQ_TYPE_LOGOUT : /* cas de demande de déconnexion */
    break;
  case REQ_TYPE_MSG : /* cas d'envoi de message */
    break;
  default :
    perror("Error handle_request(): Unkown type\n");
    exit_setup(1);
  }
}

void add_client_id(char* new_client_id)
{
  if(nb_connected == MAX_CLIENTS){
    fprintf(stderr, "Le client %s ne peut être ajouté, tableau plein\n",
	    new_client_id);
    return;
  }
  clients_ids[nb_connected] = (char*)malloc(256*sizeof(char));
  strcpy(clients_ids[nb_connected], new_client_id);
  nb_connected++;
}
