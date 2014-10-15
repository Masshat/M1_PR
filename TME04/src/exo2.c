/**
 * TP4 - EXO_2 - Remontée de valeurs par partage de mémoire
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
#include <sys/shm.h>
#include <sys/wait.h>

#ifndef N_FILS
#define N_FILS 5
#endif

int shm_id;
int values_array[N_FILS];

/**
 * Fonction executée par les fils
 */
int f_fils(int num)
{
  /* génération de la valeur aléatoire */
  srand(getpid());
  int random_val = (int)(rand() % 10);
  printf("fils %d : generated random_val = %d\n", num, random_val);

  /* attachement au segment */
  int *adr_tab;
  if ( (adr_tab = (int*)shmat(shm_id, 0, 0600)) == (void*)-1 ){
    perror("error : shmat\n");
    return EXIT_FAILURE;
  }

  /* stockage de la valeur */
  adr_tab[num] = random_val;

  /* détachement du segment */
  if ( shmdt(adr_tab) == -1 ){
    perror("error shmdt\n");
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
  /* key init */
  char path[14] = "file_msg";
  char code = 'C';
  key_t cle = ftok(path, code);

  /* création du segment de mémoire */
  int taille = N_FILS * sizeof(int);
  shm_id = shmget(cle, taille, 0666 | IPC_CREAT);

  /* création des fils */
  pid_t pid_son;
  int i;
  for(i=0; i<N_FILS; i++){
    pid_son = fork();
    if ( pid_son == -1 ){
      perror("error : fork\n");
      exit(1);
    } else if ( !pid_son ){
      return f_fils(i);
    }
  }

  /* attente de leur terminaison */
  int status;
  for(i=0; i<N_FILS; i++){
    if ( wait(&status) == -1 ){
      perror("error : wait\n");
      return EXIT_FAILURE;
    }
    if( !WIFEXITED(status) ){
      perror("error : WIFEXITED : le proc s'est mal terminé\n");
      return EXIT_FAILURE;
    }
  }

  /* attachement au segment */
  char *adr_tab;
  if ( (adr_tab = shmat(shm_id, 0, 0600)) == (void*)-1 ){
    perror("error : shmat\n");
    return EXIT_FAILURE;
  }

  /* récupération des valeurs et calcul de la somme */
  int somme = 0;
  for(i=0; i<N_FILS; i++){
    somme += (int)adr_tab[i*sizeof(int)];
  }

  /* détachement du segment */
  if ( shmdt(adr_tab) == -1 ){
    perror("error shmdt\n");
    return EXIT_FAILURE;
  }

  /* suppression du segment */
  if ( shmctl(shm_id, IPC_RMID, NULL) == -1 ){
    perror("error : msgctl\n");
    return EXIT_FAILURE;
  }

  /* affichage de la somme */
  printf("main : somme = %d\n",somme);
    
  return EXIT_SUCCESS;
}
