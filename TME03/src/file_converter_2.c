/**
 * TP3 - EXO_2 - Traitement concurrent des fichiers
 * avec un pool fixe de threads
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
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>

#ifndef NB_THREAD
#define NB_THREAD 2
#endif

/* variables partagée */
char** argtab; /* prendra les argv */
int next_file = 1; /* index du prochain fichier à traiter */
int nbfiles; /* nombre total de fichier ( argc-1 ) */
pthread_mutex_t next_file_mutex = PTHREAD_MUTEX_INITIALIZER; /* mutex */

void convert_file(char* file_name)
{
  FILE *fp1, *fp2;
  int c = 1;

  fp1= fopen (file_name, "r"); 
  fp2= fopen (file_name, "r+");
  if ((fp1== NULL) || (fp2== NULL)) {
    perror ("fopen");
    exit (1);
  }

  while (c != EOF) {
    c=fgetc(fp1);
    if (c!=EOF)
      fputc(toupper(c),fp2);
  }

  fclose (fp1);
  fclose (fp2);
}

void *thread_convert(void* arg)
{
  char *file;
  while(1){
    pthread_mutex_lock(&next_file_mutex);
    if ( next_file > nbfiles ){
      pthread_mutex_unlock(&next_file_mutex);
      break;
    }
    file = (char *)(argtab[next_file]);
    next_file++;
    pthread_mutex_unlock(&next_file_mutex);
    convert_file(file);
  }
  return NULL;
}

int main (int argc, char** argv)
{
  argtab = argv;
  nbfiles = argc-1;
  
  /* Partie creation de NB_THREAD threads */
  pthread_t tid[NB_THREAD];
  int i=-1;
  while ( ++i < NB_THREAD ){
    if ( pthread_create(&tid[i], NULL, thread_convert, (void *)&argv) ){
      perror("error : pthread_create\n");
      exit(1);
    }
  }
  
  /* Partie attente */
  i = -1;
  while ( ++i < NB_THREAD ){
    if ( pthread_join(tid[i], NULL) ){
      perror("error : pthread_join\n");
      exit(1);
    }
  }
  
  return EXIT_SUCCESS;
}
