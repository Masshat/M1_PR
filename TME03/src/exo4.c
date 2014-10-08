/**
 * TP3 - EXO_4 - Plusieurs Prod / Cons
 * J'ai tout mis dans un seul fichier cette fois
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
#include <pthread.h>

#define NB_PROD 5
#define NB_CONS 3

#define MAX_STACK_SIZE 10

int stack[MAX_STACK_SIZE]; /* la pile */

int prod_counter = 0; /* index d'accès pour les producteurs */
pthread_mutex_t mutex_prod = PTHREAD_MUTEX_INITIALIZER; /* son mutex */
pthread_cond_t cond_prod = PTHREAD_COND_INITIALIZER; /* son cond */

int cons_counter = 0; /* index d'accès pour les consomateurs */
pthread_mutex_t mutex_cons = PTHREAD_MUTEX_INITIALIZER; /* son mutex */
pthread_cond_t cond_cons = PTHREAD_COND_INITIALIZER; /* son cond */

/* un tableau de mutex, un pour chaque case, permet d'assurer
 * qu'un seul prod/cons y accèdera */
pthread_mutex_t tab_mutex[MAX_STACK_SIZE];

void Push(int c)
{
  int index; /* index du prod en cours */
  pthread_mutex_lock(&mutex_prod);
  /* cas de tableau rempli */
  while ( prod_counter == ( cons_counter + 100 ) )
    pthread_cond_wait(&cond_prod, &mutex_prod);
  index = prod_counter;
  prod_counter++;
  pthread_cond_broadcast(&cond_cons);
  pthread_mutex_lock(&tab_mutex[index % MAX_STACK_SIZE]);
  pthread_mutex_unlock(&mutex_prod);
  stack[(index % MAX_STACK_SIZE)] = c;
  pthread_mutex_unlock(&tab_mutex[index]);
}

char Pop()
{
  int index;
  pthread_mutex_lock(&mutex_cons);
  /* cas tableau vide */
  while ( prod_counter == cons_counter )
    pthread_cond_wait(&cond_cons, &mutex_cons);
  index = cons_counter;
  cons_counter++;
  pthread_cond_broadcast(&cond_prod);
  pthread_mutex_lock(&tab_mutex[index % MAX_STACK_SIZE]);
  pthread_mutex_unlock(&mutex_cons);
  int c = stack[(index % MAX_STACK_SIZE)];
  pthread_mutex_unlock(&tab_mutex[index]);
  return c;
}

void* Producteur(void* arg) { 
  int c; 
  while ((c = getchar()) != EOF) { 
    Push(c); 
  }
  return NULL;
}

void* Consommateur(void* arg) { 
  for (;;) { 
    putchar( Pop() ); 
    fflush(stdout); 
  }
  return NULL;
}

int main(int argc, char** argv)
{
  pthread_t tid_prod[NB_PROD], tid_cons[NB_CONS];
  int i;

  /* j'initialise le tableau de mutex */
  for( i=0; i<MAX_STACK_SIZE; i++){
    pthread_mutex_init(tab_mutex+i, NULL);
  }

  /* creation des threads */
  for ( i=0; i<NB_PROD; i++){
    if ( pthread_create(&tid_prod[i], NULL, Producteur, NULL) ){
      perror("error : pthread_create prod\n");
      exit(1);
    }
  }
  for ( i=0; i<NB_CONS; i++){
    if ( pthread_create(&tid_cons[i], NULL, Consommateur, NULL) ){
      perror("error : pthread_create cons\n");
      exit(1);
    }
  }
  
  /* attente de terminaison */
  for( i=0; i<NB_PROD; i++){
    if ( pthread_join(tid_prod[i], NULL) ){
      perror("error : pthread_join prod\n");
      exit(1);
    }
  }
  for( i=0; i<NB_CONS; i++){
    if( pthread_join(tid_cons[i], NULL) ){
      perror("error : pthread_join cons\n");
      exit(1);
    }
  }

  return EXIT_SUCCESS;
}

