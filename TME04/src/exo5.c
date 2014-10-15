/**
 * TP4 - EXO_5 - Rendez-vous par sémaphores
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @copyright (c) 2014, toumlilt
 *
 * @version 1.0
 * @package toumlilt/M1/PR
 */

#define _SVID_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <string.h>

#ifndef NB_PCS
#define NB_PCS 3
#endif

#define SEM_BARRIER 0 /* index sem pour la barrière */
#define MUTEX 1 /* index of control sem */
#define NB_SEM 2 /* nombre de sémaphores ... */

int sem_id; /* id de l'ens de sem */

/**
 * Primitive P()
 */
void P(int sem_num)
{
  struct sembuf op;
  op.sem_num = sem_num;
  op.sem_op = -1;
  op.sem_flg = SEM_UNDO;
  semop(sem_id, &op, 1);
}

/**
 * Primitive V()
 */
void V(int sem_num)
{
  struct sembuf op;
  op.sem_num = sem_num;
  op.sem_op = 1;
  op.sem_flg = SEM_UNDO;
  semop(sem_id, &op, 1);
}

void wait_barrier(int nb_proc)
{
  /* notification sur MUTEX */
  V(MUTEX);
  /* attente sur barrière */
  P(SEM_BARRIER);
}

/**
 * fonction executée par les fils
 */
void process(int nb_proc)
{
  printf("avant barrière\n");
  wait_barrier(nb_proc);
  printf("après barrière\n");
  exit(0);
}

int main(int argc, char** argv)
{
  /* init key */
  char* path = "/tmp";
  key_t cle = ftok(path, 7);

  /* création de l'ensemble de sémaphores */
  sem_id = semget(cle, NB_SEM, 0666 | IPC_CREAT);

  /* init de la valeur des sem */
  semctl(sem_id, SEM_BARRIER, SETVAL, 0);
  semctl(sem_id, MUTEX, SETVAL, 0);
  
  /* création des fils qui vont lancer process() */
  pid_t pid_son;
  int i;
  for(i=0; i<NB_PCS; i++){
    pid_son = fork();
    if ( pid_son == -1 ){
      perror("error : fork\n");
      exit(1);
    } else if ( !pid_son ){
      process(NB_PCS);
    }
  }

  /* attente de NB_PCS notifications sur Mutex */
  for(i=0; i<NB_PCS; i++)
    P(MUTEX);

  /* ouverture de la barrière pour les NB_PCS */
  for(i=0; i<NB_PCS; i++)
    V(SEM_BARRIER);

  /* attente de bonne terminaison des fils */
  int status;
  for(i=0; i<NB_PCS; i++){
    if ( wait(&status) == -1 ){
      perror("error : wait\n");
      return EXIT_FAILURE;
    }
    if( !WIFEXITED(status) ){
      perror("error : WIFEXITED : le proc s'est mal terminé\n");
      return EXIT_FAILURE;
    }
  }

  /* destruction des sems */
  semctl(sem_id, SEM_BARRIER, IPC_RMID, 0);
  semctl(sem_id, MUTEX, IPC_RMID, 0);
  
  /* mot de fin */
  printf("Main : Fin.\n");
  
  return EXIT_SUCCESS;
}
