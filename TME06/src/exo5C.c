/**
 * TP6 - EXO_5 - Fonctions POSIX vs fonctions C 
 * Version fonctions C
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

#define NB_FILS 3

/**
 * fonction exécutée par les fils
 */
void fils_fx(FILE *f)
{
  int c;
  while(!feof(f)){
    c = fgetc(f);
    printf("Pid%d a lu %c\n", getpid(), c);
  }
}

int main( int argc, char** argv)
{
  /* vérification des nb de params */
  if( argc != 2 ){
    fprintf(stderr, "Error main() : il faut passer le nom fichier en paramètre\n");
    return EXIT_FAILURE;
  }
  
  /* ouverture du fichier */
  FILE* f = fopen(argv[1], "r");
  if( !f ){
    perror("Error: fopen()\n");
    return EXIT_FAILURE;
  }
  
  /* création des fils */
  int i;
  pid_t pid_son;
  for(i=0; i<NB_FILS; i++){
    pid_son = fork();
    if( pid_son == -1 ){
      perror("Error: fork()\n");
      return EXIT_FAILURE;
    }
    else if( !pid_son ){
      fils_fx(f);
      return EXIT_SUCCESS;
    }
  }

  /* attente de terminaison des fils */
  for(i=0; i<NB_FILS; i++){
    wait(NULL);
  }

  /* fermeture du fichier */
  if( fclose(f) ){
    perror("Error: fclose()\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
