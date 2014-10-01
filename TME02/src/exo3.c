/**
 * TP2 - EXO3 - Synchronisation
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @copyright (c) 2014, toumlilt
 *
 * @version 1.0
 * @package toumlilt/M1/PR
 */

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/**
 * Nombre de threads legers
 */
#ifndef N
#define N 5
#endif

int somme = 0;
int nb_somme = 0;
pthread_mutex_t mutex_somme = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_print = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_print = PTHREAD_COND_INITIALIZER;

void *thread_rand(void *arg)
{
  int* i = (int*)malloc(sizeof(int));
  *i = *(int *)arg;
  free(arg);
  
  int random_val = (int)(10*((double)rand())/ RAND_MAX);
  printf("Thread num%d, tid%d, random_val%d\n",*i, (int) pthread_self(), random_val);
  
  pthread_mutex_lock(&mutex_somme);
  somme += random_val;
  pthread_mutex_unlock(&mutex_somme);

  pthread_mutex_lock(&mutex_print);
  nb_somme++;
  pthread_cond_signal(&cond_print);
  pthread_mutex_unlock(&mutex_print);

  (*i) *= 2;
  pthread_exit((void*)i);
  return NULL;
}

void *print_thread(void *arg)
{
  pthread_mutex_lock(&mutex_print);
  while(nb_somme < N)
    pthread_cond_wait(&cond_print, &mutex_print);
  pthread_mutex_unlock(&mutex_print);
  
  printf("Somme valeurs aleatoires = %d\n", somme);
  pthread_exit((void *)0);
  return ((void *)NULL);
}

int main(int argc, char** argv)
{
  pthread_t tid[N];
  pthread_t tid_print;
  int *pt_ind;
  
  if ( pthread_create(&tid_print, NULL, print_thread, NULL)){
    perror("error : pthread_create\n");
    exit(1);
  }
  
  int i = -1;
  while ( ++i < N ){
    pt_ind = (int*) malloc(sizeof(i));
    *pt_ind = i;
    if ( pthread_create(&(tid[i]), NULL, thread_rand, (void *)pt_ind)){
      perror("error : pthread_create\n");
      exit(1);
    }
  }
  
  i = -1;
  int* s_retour;
  while ( ++i < N ){
    if ( pthread_join(tid[i], (void **)&s_retour) ){
      perror("error : pthread_join\n");
      exit(1);
    }
    printf("Main : thread %d terminé, status = %d\n",i,*s_retour);
    free(s_retour);
  }
  if ( pthread_join(tid_print, NULL) ){
    perror("error : pthread_join\n");
    exit(1);
  }
  printf("Tous les threads sont terminés\n");
  return EXIT_SUCCESS;
}
