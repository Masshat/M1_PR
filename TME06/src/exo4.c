/**
 * TP6 - EXO_4 - Remontée de valeurs par échanges dans un fichier.
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
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

/*
 * On reprend l'exo 2 du TME4, avec transmission des valeurs aléatoires
 * par écriture dans un fichier
 *
 * NB: Fichier utilisé: src/testfiles/tmpfileexo4, sera vidé à la création.
 */
#define NB_FILS 5

char* filename="src/testfiles/tmpfileexo4";

sem_t mutex;

/**
 * Fonction exécutée par les fils,
 * génération de la random_val qui sera écrite dans le fichier
 */
void fx_fils(int index)
{
  /* génération de la random_val */
  srand(getpid());
  int random_val = (int)((int)rand() % 10);
  printf("Fils %d : random_val=%d\n", index, random_val);

  /* int to string */
  char buffer[2]; /* random_val est toujours un chiffre */
  sprintf(buffer, "%d ", random_val);

  /* ouverture du fichier */
  int fd;
  if( (fd = open(filename, O_WRONLY)) == -1){
    fprintf(stderr, "Error in fx_fils() : open()\n");
    exit(1);
  }

  /* procédure d'écriture, protégée par un sem */
  sem_wait(&mutex);

  /* déplacement du descripteur à la fin du fichier */
  if( lseek(fd, 0, SEEK_END) == -1){
    fprintf(stderr, "Error in fx_fils() : lseek()\n");
    exit(1);
  }

  /* écriture */
  if( write( fd, buffer, 2*sizeof(char)) == -1 ){
    fprintf(stderr, "Error in fx_fils() : fils%d write()\n", index);
    exit(1);
  }

  /* on libère l'accès */
  sem_post(&mutex);

  /* fermeture du fichier */
  if ( close(fd) ){
    fprintf(stderr, "Error in fx_fils() : close()\n");
    exit(1);
  }
}

int main(int argc, char** argv)
{
  /* init du sem */
  if( sem_init(&mutex, 1, 1) == -1){
    perror("Error: sem_init\n");
    return EXIT_FAILURE;
  }

  /* ouverture / création du fichier */
  int fd;
  if( (fd=open(filename, O_RDWR | O_CREAT | O_TRUNC, 0600)) == -1){
    fprintf(stderr, "Error in main() : open()\n");
    return EXIT_FAILURE;
  }

  /* création des fils */
  int i, pid_fils;
  for(i=0; i<NB_FILS; i++){
    if( (pid_fils = fork()) == -1){
      fprintf(stderr, "Error in main() : fork()\n");
      return EXIT_FAILURE;
    } else if( pid_fils == 0){
      fx_fils(i); /* le fils fait ce qu'il a à faire */
      exit(0); /* puis se termine */
    }
  }

  /* attente et vérification de bonne terminaison des fils */
  int status;
  for(i=0; i<NB_FILS; i++){
    wait(&status);
    if( !WIFEXITED(status) || WEXITSTATUS(status) ){
      fprintf(stderr, "Error in main() : un fils s'est mal terminé, ou s'est terminé avec erreur\n");
      return EXIT_FAILURE;
    }
  }

  /* lecture et somme des valeurs */
  int somme=0, lu;
  char buffer[2];
  for(i=0; i<NB_FILS; i++){
    if( read(fd, buffer, 2) == -1){
      fprintf(stderr, "Error in main() : read()\n");
      return EXIT_FAILURE;
    }
    sscanf(buffer, "%d ", &lu);
    somme += lu;
  }

  /* fermeture du fichier */
  if( close(fd) ){
    fprintf(stderr, "Error in main() : close()\n");
    return EXIT_FAILURE;
  }

  /* destruction du sem */
  sem_destroy(&mutex);

  /* affichage */
  printf("Main : somme des valeurs aléatoires = %d\n", somme);

  return EXIT_SUCCESS;
}
