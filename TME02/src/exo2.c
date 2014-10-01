/**
 * TP2 - EXO2 - Exclusion mutuelle
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

#ifndef N
#define N 5
#endif

int somme = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_rand(void *arg)
{
  int* i = (int*)malloc(sizeof(int));
  *i = *(int *)arg;
  free(arg); /* alloué dans main */
  
  int random_val = (int)(10*((double)rand())/ RAND_MAX);
  printf("Thread num%d, tid%d, random_val = %d\n",*i, (int) pthread_self(), random_val);
  pthread_mutex_lock(&mutex);
  somme += random_val;
  pthread_mutex_unlock(&mutex);
  (*i) *= 2;
  pthread_exit((void*)i);
  return NULL;
}

int main(int argc, char** argv)
{
  /* partie creation */
  pthread_t tid[N];
  int *pt_ind;
  int i = -1;
  while ( ++i < N ){
    pt_ind = (int*) malloc(sizeof(i));
    *pt_ind = i;
    if ( pthread_create(&(tid[i]), NULL, thread_rand, (void *)pt_ind)){
      perror("error : pthread_create\n");
      exit(1);
    }
  }

  /* partie attente */
  int *s_retour;
  i = -1;
  while ( ++i < N ){
    if ( pthread_join(tid[i], (void **)&s_retour) ){
      perror("error : pthread_join\n");
      exit(1);
    }
    printf("Main : thread %d terminé, status = %d\n",i,*s_retour);
    free(s_retour);
  }
  printf("Tous les threads sont terminés\n");
  printf("Somme valeurs aleatoires = %d\n", somme);
  return EXIT_SUCCESS;
}
