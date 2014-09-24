/**
 * TP1 - Exo1 - Passage de parametres, compilation, makefile
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @copyright (c) 2014, toumlilt
 *
 * @version 1.0
 * @package toumlilt/M1/PR
 */

#include <stdio.h>
#include <stdlib.h>

#include <max_func.h>

/**
 * Recherche du max dans un tableau d'entiers
 * recherche classique en O(n)
 * 
 * @param int* vecteur tableau d'entiers
 * @param int taille taille du tableau
 * @return int l'entier max du tableau
 */
int max_func(int* vecteur, int taille)
{
  if ( !taille ) {
    perror("max_func : taille nulle\n");
    exit(EXIT_FAILURE);
  }
  
  int max = vecteur[0];
  int i;
  for(i=1; i<taille; i++)
    if ( vecteur[i] > max )
      max = vecteur[i];
  return max;
}
