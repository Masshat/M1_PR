/**
 * TP6 - EXO_2 - Recopie de fichier par lectures/écritures
 *
 * Ce programme prend en params deux noms de fichiers
 * $ mycp <fic1> <fic2>
 * Il recopie intégralement le contenu de fic1 dans fic2.
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
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define BUFFER_SIZE 256

/**
 * Ce programme prend en params deux noms de fichiers
 * $ mycp <fic1> <fic2>
 * Il recopie intégralement le contenu de fic1 dans fic2.
 * Il y a trois conditions pour que le programme se termine sans erreurs :
 * 1- le fichier fic1 doit exister.
 * 2- le fichier fic2 ne doit pas exister.
 * 3- et fic1 est un fichier régulier.
 */
int main(int argc, char** argv){
  
  /* vérification du nombre de paramètres */
  if ( argc != 3 ){
    fprintf(stderr, "Error: Usage: mycp <fic1> <fic2>\n");
    return EXIT_FAILURE;
  }

  int fd1, fd2;

  /* Le fichier fic1 doit exister */
  if ( access(argv[1], F_OK) ) {
    fprintf(stderr, "Error : fic1 innexistant\n");
    return EXIT_FAILURE;
  }

  /* Le fichier fic2 ne doit pas exister */
  if ( !access(argv[2], F_OK) ){
    fprintf(stderr, "Error : fic2 existe déjà\n");
    return EXIT_FAILURE;
  }

  /* fic1 doit être un fichier régulier */
  struct stat buf;
  if( stat(argv[1], &buf) == -1){
    fprintf(stderr, "error : stat\n");
    return EXIT_FAILURE;
  }
  if( !S_ISREG(buf.st_mode) ){
    fprintf(stderr, "error : fic1 n'est pas un fichier régulier\n");
    return EXIT_FAILURE;
  }

  /* ouverture des fichiers */
  if( (fd1=open(argv[1], O_RDONLY)) == -1 || 
      (fd2=open(argv[2], O_WRONLY | O_CREAT, 0666)) == -1){
    fprintf(stderr, "error : open\n");
    return EXIT_FAILURE;
  }

  /* recopie */
  char buffer[BUFFER_SIZE];
  int nb_lu;
  while( (nb_lu=read(fd1, buffer, BUFFER_SIZE) ) > 0){
    if( write(fd2, buffer, nb_lu) == -1 ){
      fprintf(stderr, "error : write\n");
      exit(1);
    }
  }

  /* fermeture du fic1 */
  if ( close(fd1) == -1 ){
    fprintf(stderr, "error : close file1\n");
    exit(1);
  }

  /* fermeture du fic2 */
  if( close(fd2) == -1 ){
    fprintf(stderr, "error : close file2\n");
    exit(1);
  }

  return EXIT_SUCCESS;
}
