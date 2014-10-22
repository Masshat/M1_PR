/**
 * TP5 - EXO_2 - Rendez-vous par sémaphores
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
#include <sys/types.h>
#include <semaphore.h>
#include <wait.h>
#include <fcntl.h>
#include <sys/stat.h>

#define N_PCS 5

sem_t *mutex; /* semaphore pour le pcs qui va agir differemment */
sem_t *sem_barrier; /* semaphore barrière */
int i_fils; /* index du fils à chaque fork, pour l'indentification */

/**
 * Principe de la barrière sans compteur :
 * le premier fils va toujours agir différemment des autres
 * il sera le délégué qui va attendre l'entrée des N autres fils
 * chaque autre fils signalera son entrée par un post sur mutex
 */
void wait_barrier(int N){
  /* Le premier fils agit differement ! */
  if ( i_fils == 0 ){
    while ( --N != 0 ) 
      sem_wait(mutex);
    /* Quand tout le monde aura marqué son entrée */
    sem_post(sem_barrier); /* on ouvre la barrière pour les sorties */
  } else {
    /* Chacun marque son entrée */
    sem_post(mutex);
  }
  sem_wait(sem_barrier); /* Attente sur la barrière */
  sem_post(sem_barrier); /* Libération de la barrière pour le suivant */
}

/**
 * Fonction fournie dans l'exo
 * Sera exécutée par chaque fils
 */
void process (int NB_PCS)
{
  printf("avant barrière\n");
  wait_barrier(NB_PCS);
  printf("après barrière\n");
  exit(0);
}

int main(int argc, char** argv)
{

  /* création des sémaphores nommées */
  if ( (mutex = sem_open("mysem1", O_CREAT | O_RDWR, 0600, 0)) == SEM_FAILED){
    perror("error : sem_open\n");
    return EXIT_FAILURE;
  }
  if ( (sem_barrier = sem_open("mysem2", O_CREAT | O_RDWR, 0600, 0)) == SEM_FAILED){
    perror("error : sem_open\n");
    return EXIT_FAILURE;
  }

  /* init des semaphores */
  if( sem_init(mutex, 1, 0) == -1){
    perror("error : sem_init mutex\n");
    return EXIT_FAILURE;
  }
  if( sem_init(sem_barrier, 1, 0) == -1){
    perror("error : sem_init sem_barrier\n");
    return EXIT_FAILURE;
  }

  /* creation des fils */
  pid_t pid_son;
  int i;
  for( i=0; i<N_PCS; i++){
    i_fils = i;
    if( (pid_son=fork()) == -1){
      perror("error : fork creation\n");
      return EXIT_FAILURE;
    } else if ( pid_son == 0 ){
      process(N_PCS);
      exit(0);
    }
  }

  /* attente de terminaison des fils */
  int status;
  for(i=0; i<N_PCS; i++){
    if ( wait(&status) == -1 ){
      perror("error : wait\n");
      return EXIT_FAILURE;
    }
    if( !WIFEXITED(status) ){
      perror("error : WIFEXITED : le proc s'est mal terminé\n");
      return EXIT_FAILURE;
    }
  }

  /* fermeture des sems */
  if( sem_close(mutex) == -1){
    perror("error : sem_close\n");
    return EXIT_FAILURE;
  }
  if( sem_close(sem_barrier) == -1){
    perror("error : sem_close\n");
    return EXIT_FAILURE;
  }

  /* suppression des sems */
  if(sem_unlink("mysem1") == -1){
    perror("error : sem_unlink\n");
    return EXIT_FAILURE;
  }
  if(sem_unlink("mysem2") == -1){
    perror("error : sem_unlink\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
