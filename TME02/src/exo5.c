/**
 * TP2 - EXO5 - Synchronisation par broadcast 
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

#define NB_THREADS 3

int nb_done = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond= PTHREAD_COND_INITIALIZER;

void wait_barrier(int N)
{
  pthread_mutex_lock(&m);
  nb_done++;
  if ( nb_done != N)
    pthread_cond_wait(&cond,&m);
  else
    pthread_cond_broadcast(&cond);
  pthread_mutex_unlock(&m);
}

void* thread_func (void *arg)
{
  printf ("avant barriere\n");
  wait_barrier (NB_THREADS);
  printf ("après barriere\n");
  pthread_exit ( (void*)0);
}


int main(int argc, char** argv)
{
  int i = -1;
  pthread_t tid[NB_THREADS];

  /* création */
  while ( ++i < NB_THREADS ){
    if ( pthread_create(tid+i, NULL, thread_func, NULL)){
      perror("error : pthread_create\n");
      exit(1);
    }
  }

  /* attente */
  i=-1;
  while ( ++i < NB_THREADS ){
    if ( pthread_join(tid[i], NULL) ){
      perror("error : pthread_join\n");
      exit(1);
    }
  }
  
  return EXIT_SUCCESS;
}
