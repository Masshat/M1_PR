/**
 * TP2 - EXO4 - Détachemnt des threads
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

int nb_end = 0;
pthread_mutex_t mutex_end = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_end = PTHREAD_COND_INITIALIZER;

void *thread_rand(void *arg)
{
  
  int random_val = (int)(10*((double)rand())/ RAND_MAX);
  printf("Thread tid%d, random_val%d\n", (int) pthread_self(), random_val);
  
  pthread_mutex_lock(&mutex_somme);
  somme += random_val;
  pthread_mutex_unlock(&mutex_somme);

  pthread_mutex_lock(&mutex_print);
  nb_somme++;
  pthread_cond_signal(&cond_print);
  pthread_mutex_unlock(&mutex_print);

  pthread_mutex_lock(&mutex_end);
  nb_end++;
  pthread_cond_signal(&cond_end);
  pthread_mutex_unlock(&mutex_end);
  
  pthread_exit((void*)NULL);
  return NULL;
}

void *print_thread(void *arg)
{
  pthread_mutex_lock(&mutex_print);
  while(nb_somme < N)
    pthread_cond_wait(&cond_print, &mutex_print);
  pthread_mutex_unlock(&mutex_print);
  
  printf("Somme valeurs aleatoires = %d\n", somme);

  pthread_mutex_lock(&mutex_end);
  nb_end++;
  pthread_cond_signal(&cond_end);
  pthread_mutex_unlock(&mutex_end);
  
  pthread_exit((void *)0);
  return ((void *)NULL);
}

int main(int argc, char** argv)
{
  pthread_t tid[N];
  pthread_t tid_print;
  
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
  
  if ( pthread_create(&tid_print, &attr, print_thread, NULL)){
    perror("error : pthread_create\n");
    exit(1);
  }
  
  int i = -1;
  while ( ++i < N ){
    if ( pthread_create(&(tid[i]), &attr, thread_rand, NULL)){
      perror("error : pthread_create\n");
      exit(1);
    }
  }

  /* attente de terminaison */
  pthread_mutex_lock(&mutex_end);
  while( nb_end < (N+1) )
    pthread_cond_wait(&cond_end, &mutex_end);
  pthread_mutex_unlock(&mutex_end);
  
  printf("Tous les threads sont terminés\n");
  return EXIT_SUCCESS;
}
