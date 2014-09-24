/**
 * TP1 - Exo2 - N Processus Fils
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

/**
 * Création de n_fils processus fils.
 * NB : Si la création d'un proc fils échoue, la fonction n'essaie pas de créer les autres fils.
 *
 * @param int nb_fils nombre de fils à créer
 * @return int Le nombre de fils créés pour le processus père, 0 pour le fils, -1 dans le cas où aucun fils n'a été créé
 */
int nfork(int nb_fils)
{
  int i=-1; /* Nombre de fils créés à chaque tour de boucle */
  pid_t fils; /* Pour récupérer proprement le pid du fils créé */
  while( ++i < nb_fils ){
    fils = fork();
    if ( fils == -1 ){
      i--; /* ce fils n'a pas été créé ... */
      break;
    } else if ( !fils ){
      return 0;
    }
  }
  return ( !i ) ? -1 : i; /* On retourne -1 si aucun fils n'a été créé, leur nombre sinon. */
}
    
int main(int argc, char** argv)
{
  int p;
  int i=1; int N = 3;
  do {
    p = nfork(i);
    if ( p != 0)
      printf("%d \n",p);
  } while ((p == 0) && (++i<N)); /* le prof a changé ça en ++i<N mais j'avais déjà soumis ... */
  printf("FIN \n");
  return EXIT_SUCCESS;
}

