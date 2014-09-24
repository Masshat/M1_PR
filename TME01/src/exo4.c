/**
 * TP1 - Exo4 - Chaine de processus
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

#ifndef NB_PROC
#define NB_PROC 5
#endif


int main(int argc, char** argv)
{
  int i = -1; /* index compteur de processus */
  pid_t pids_array[NB_PROC]; /* tableau de tous les pids de la chaîne */
  pid_t pid_fils; /* pid du fils créé */
  int status; /* status de retour du fils à attendre */
  int valeur_aleatoire; /* généré par le dernier fils puis repassé à travers la chaîne en code de retour */

  /* creation de la chaîne de processus */
  while ( ++i < NB_PROC ) {
    pids_array[i] = getpid();
    if ( (pid_fils=fork()) == -1 ){
      perror("fork\n");
      exit(1);
    } else if ( pid_fils ) {
      break;
    }
  }

  /* attente & affichages */
  if ( pid_fils ) { /* tous sauf le dernier fils */
    wait(&status);
    valeur_aleatoire = WEXITSTATUS(status);
  } else { /* dernier fils */
    printf("Dernier fils : affichage de tous les pids :\n");
    for ( i=0; i < NB_PROC; i++ )
      printf("pid[%d]=%d\n",i,pids_array[i]);
    printf("Fin affichage\n");
    valeur_aleatoire = (int)(rand() / (((double) RAND_MAX +1) /100));
  }

  /* mot de fin */
  printf("Proc %d : pid%d, pere%d, fils%d : se termine\n",i,getpid(),getppid(),pid_fils);

  /* le boss a toujours un mot de plus */
  if ( getpid() == pids_array[0] ){
    printf("Valeur aleatoire : %d\n", valeur_aleatoire);
    valeur_aleatoire = 0; /* le proc principal retourne 0 ... */
  }
  return valeur_aleatoire;
}
