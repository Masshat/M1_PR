/**
 * TP1 - Exo1 - Passage de parametres, compilation, makefile
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

#include <max_func.h>

/**
 * Parse un tableau de char* en tableau de int
 * va servir pour l'exo à la conversion de argv
 *
 * @param char** array le tableau à parser
 * @param int size taille du tableau
 * @return int* le tableau parsé en entiers
 */
int* parse_array(char** array, int size)
{
  int* parsed_array = (int*)malloc(size*sizeof(int));
  int i = 0;
  for(; i < size; i++){
    /* je préfère sscanf à atoi pour la portabilité ;-) */
    if ( !sscanf(array[i], "%d", &parsed_array[i]) ){
      fprintf(stderr,"parse array : vecteur[%d] n'est pas un entier\n",i);
      exit(EXIT_FAILURE);
    }
  }
  return parsed_array;
}

int main(int argc, char** argv)
{
  int* parsed_array = parse_array(argv+1, argc-1);
  int max = max_func(parsed_array, argc-1);
  printf("Le maximum est : %d\n", max);

  free(parsed_array);
  
  return EXIT_SUCCESS;
}
