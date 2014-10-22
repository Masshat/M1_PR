/**
 * TP5 - EXO_1 - Remontée de valeurs par partage de mémoire
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
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>

#define N 5

int *sp;

/**
 * Fonction executée par les fils
 * Stock la valeur alétoire générée dans sp[num]
 */
int fx_fils(int num){
  srand(getpid());
  sp[num] = (int)(rand() % 10);
  return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
  pid_t pid_son;
  int i;
  int fd;

  /* creation de la shm */
  if ( (fd=shm_open("my_shm", O_RDWR | O_CREAT, 0600)) == -1){
    perror("error : shm_open\n");
    return EXIT_FAILURE;
  }

  /* allocation de la taille du segment */
  if( ftruncate(fd, N*sizeof(int)) == -1){
    perror("error: ftruncate\n");
    return EXIT_FAILURE;
  }

  /* mapper le segment de mémoire */
  if( (sp = (int*)mmap(NULL, N*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
    perror("error : mmap\n");
    return EXIT_FAILURE;
  }
  
  /* creation des fils */
  for( i=0; i<N; i++){
    if( (pid_son=fork()) == -1){
      perror("error : fork creation\n");
      return EXIT_FAILURE;
    } else if ( pid_son == 0 ){
      return fx_fils(i);
    }
  }
   
  /* attente de bonne terminaison des fils */
  int status;
  for(i=0; i<N; i++){
    if ( wait(&status) == -1 ){
      perror("error : wait\n");
      return EXIT_FAILURE;
    }
    if( !WIFEXITED(status) ){
      perror("error : WIFEXITED : le proc s'est mal terminé\n");
      return EXIT_FAILURE;
    }
  }

  /* récupération des valeurs */
  int somme = 0;
  for(i=0; i<N; i++){
    somme += sp[i];
    printf("Pere a recu la val %d du fils %d\n", sp[i], i);
  }
  
  /* détacher le segment */
  if ( munmap(sp, N*sizeof(int)) == -1){
    perror("error : munmap\n");
    return EXIT_FAILURE;
  }

  /* destruction du segment */
  if ( shm_unlink("my_shm") == -1 ){
    perror("error : shm_unlink\n");
    return EXIT_FAILURE;
  }
  
  /* mot de fin */
  printf("somme des valeurs = %d\n",somme);

  return EXIT_SUCCESS;
}
