/**
 * TP3 - EXO_1 - Traitement concurrent des fichiers avec un nombre illimité de threads
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

/**
 * Fonction de conversion
 * executée par les threads
 */
void *thread_convert(void* arg)
{
  FILE *fp1, *fp2; 
  int c = 1;

  fp1= fopen ((char *)arg, "r"); 
  fp2= fopen ((char *)arg, "r+");
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

  return NULL;
}

int main (int argc, char ** argv)
{
  int i=0;
  
  /* Partie creation de threads */
  pthread_t tid[argc-1];
  while ( ++i < argc ){
    if ( pthread_create(&tid[i-1], NULL, thread_convert, (void *)argv[i]) ){
      perror("error : pthread_create\n");
      exit(1);
    }
  }
  
  /* Partie attente */
  i = 0;
  while ( ++i < argc ){
    if ( pthread_join(tid[i-1], NULL) ){
      perror("error : pthread_join\n");
      exit(1);
    }
  }
  
  return EXIT_SUCCESS;
}
