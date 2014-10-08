/**
 * TP3 - EXO_3 - producteur / consommateur
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

#include <thread_stack.h>

#define MAX_STACK_SIZE 100

int stack[MAX_STACK_SIZE];

int stack_size = 0;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void Push(int c)
{
  pthread_mutex_lock(&m);
  if ( stack_size == 100 )
    pthread_cond_wait(&cond, &m);
  stack[stack_size++] = c;
  if ( pthread_cond_signal(&cond) ){
    perror("error signal push\n");
    exit(1);
  }
  pthread_mutex_unlock(&m);
}

char Pop()
{
  pthread_mutex_lock(&m);
  if ( stack_size == 0 ){
    if ( pthread_cond_wait(&cond, &m) ){
      perror("pop pthread_cond_wait : error\n");
      exit(1);
    }
  }
  int c = stack[--stack_size];
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&m);
  return c;
}
