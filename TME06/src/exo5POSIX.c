/**
 * TP6 - EXO_5 - Fonctions POSIX vs fonctions C 
 * Version fonctions Posix
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

#define NB_FILS 3

/**
 * fonction exécutée par les fils
 */
void fils_fx(int fd)
{
  char buff[1];
  while ( read(fd, buff, sizeof(char)) ){
    printf("Pid%d a lu %c\n", getpid(), buff[0]);
  }
}

int main(int argc, char** argv)
{
  /* vérification des nb de params */
  if( argc != 2 ){
    fprintf(stderr, "Error main() : il faut passer le nom fichier en paramètre\n");
    return EXIT_FAILURE;
  }

  /* ouverture du fichier */
  int fd = open(argv[1], O_RDONLY);
  if( fd == -1 ){
    perror("Error: open()\n");
    exit(1);
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
      fils_fx(fd);
      return EXIT_SUCCESS;
    }
  }

  /* attente de terminaison des fils */
  for(i=0; i<NB_FILS; i++){
    wait(NULL);
  }
  
  /* fermeture du fichier */
  if( close(fd) == -1 ){
    perror("Error: close()\n");
    exit(1);
  }

  return EXIT_SUCCESS;
}
