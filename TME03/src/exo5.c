/**
 * TP3 - EXO5 - Chaînes de threads et signaux
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
#include <signal.h>

#define N 5

pthread_t tids[N];

/* mutex & cond, pour la notification de création de toute la chaîne */
char init_flag = 0;
pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t init_cond = PTHREAD_COND_INITIALIZER;

/* mutex pour attente des fils */
char wait_flag = 0;
pthread_mutex_t wait_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait_cond = PTHREAD_COND_INITIALIZER;

void* f_threads(void *arg)
{
  int* in = (int*)arg;
  int i = *in;
  i++;
  
  if( i < N ){
    if( pthread_create(tids+i, NULL, f_threads, (void*)&i) ){
      fprintf(stderr, "Error : pthread_create\n");
      exit(1);
    }
  }
  else if( i == N){
    pthread_mutex_lock(&init_mutex);
    init_flag = 1;
    pthread_cond_signal(&init_cond);
    pthread_mutex_unlock(&init_mutex);
  }

  pthread_mutex_lock(&wait_mutex);
  while(!wait_flag)
    pthread_cond_wait(&wait_cond, &wait_mutex);
  pthread_mutex_unlock(&wait_mutex);

  return NULL;
}

int main(int argc, char** argv)
{
  /* masque des signaux */
  sigset_t ens;
  sigfillset(&ens);
  pthread_sigmask(SIG_SETMASK, &ens, NULL);
  
  /* lancement de la création de la chaîne */
  int i = 0;
  if( pthread_create(tids, NULL, f_threads, (void*)&i) ){
    fprintf(stderr, "Error : pthread_create\n");
    exit(1);
  }

  /* attente de fin de création */
  pthread_mutex_lock(&init_mutex);
  if(!init_flag)
    pthread_cond_wait(&init_cond, &init_mutex);
  pthread_mutex_unlock(&init_mutex);
  printf("Tous mes descendants sont créés\n");

  /* attente SIGINT */
  sigemptyset(&ens);
  sigaddset(&ens, SIGINT);
  sigwait(&ens, NULL);
  
  /* broadcast pour reprise d'activité des fils */
  pthread_mutex_lock(&wait_mutex);
  wait_flag = 1;
  pthread_cond_broadcast(&wait_cond);
  pthread_mutex_unlock(&wait_mutex);

  /* attente de terminaison */
  for(i=0; i<N; i++){
    if( pthread_join(tids[i], NULL) ){
      fprintf(stderr, "Error : pthread_join\n");
      exit(1);
    }
  }
  
  return EXIT_SUCCESS;
}
