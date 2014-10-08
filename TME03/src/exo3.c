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
  pthread_t tid_prod, tid_cons;
  if ( pthread_create(&tid_prod, NULL, Producteur, NULL) ||
       pthread_create(&tid_cons, NULL, Consommateur, NULL) ){
    perror("error : pthread_create\n");
    exit(1);
  }
  /* attente */
  if ( pthread_join(tid_prod, NULL) || pthread_join(tid_cons, NULL) ){
    perror("error : pthread_join\n");
    exit(1);
  }
  return EXIT_SUCCESS;
}

