/**
 * TP6 - EXO_3 - Recopie de fichier par mapping
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
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/**
 * On reprend l'exo précédent, mais avec des règles contraignantes :
 * - toute opération de lecture est interdite - OK
 * - une seule écriture est autorisée - OK
 * - les fonctions fstat et mmap doivent figurer dans la solution - OK
 */
int main(int argc, char** argv)
{
  char* buffer;
  struct stat s;

  /* vérification du nombre d'arguments */
  if( argc != 3 ){
    fprintf(stderr, "Error : nb params incorrect : exo3 <fic1> <fic2>\n");
    return EXIT_FAILURE;
  }

  /* le fichier fic1 doit exister */
  if( access(argv[1], F_OK) ) {
    fprintf(stderr, "Error : fic1 innexistant\n");
    return EXIT_FAILURE;
  }

  /* le fichier fic2 ne doit pas exister */
  if( !access(argv[2], F_OK) ){
    fprintf(stderr, "Error : fic2 existe déjà\n");
    return EXIT_FAILURE;
  }

  /* fic1 doit être un fichier régulier */
  if( stat(argv[1], &s) == -1){
    fprintf(stderr, "Error : stat\n");
    return EXIT_FAILURE;
  }
  if( !S_ISREG(s.st_mode) ){
    fprintf(stderr, "error : fic1 n'est pas un fichier régulier\n");
    return EXIT_FAILURE;
  }

  /* ouverture des fichiers */
  int fd1, fd2;
  if ( (fd1 = open(argv[1], O_RDONLY)) == -1 ||
       (fd2 = open(argv[2], O_WRONLY | O_CREAT, 0666)) == -1){
    fprintf(stderr, "Error : open\n");
    return EXIT_FAILURE;
  }

  /* on s'y attache */
  if( (buffer = mmap(0, s.st_size, PROT_READ, MAP_PRIVATE, fd1, 0)) == (void*)-1){
    fprintf(stderr, "Error : mmap\n");
    return EXIT_FAILURE;
  }
  
  /* on écrit d'un coup dans fd2 */
  write(fd2, (void*)buffer, s.st_size);
  
  /* on détache */
  if( munmap(buffer, s.st_size) == -1){
    fprintf(stderr, "Error : munmap\n");
    return EXIT_FAILURE;
  }

  /* et on ferme */
  if( close(fd1) || close(fd2) ){
    fprintf(stderr, "Error : fclose\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
