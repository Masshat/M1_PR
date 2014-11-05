/**
 * TP6 - EXO_7 - Inverseur de contenu en utilisant lseek
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @copyright (c) 2014, toumlilt
 *
 * @version 1.0
 * @package toumlilt/M1/PR
 */

#define _XOPEN_SOUCE 700

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
  /* vérification du nombre de paramètres */
  if( argc != 2 ){
    perror("Usage: exo7 <filename>\n");
    return EXIT_FAILURE;
  }

  /* init */
  char* srcFile = argv[1];
  char resFile[128];
  memset(resFile, '\0', 128);
  strcat(resFile, srcFile);
  strcat(resFile, ".inv");

  /* ouverture du fichier source */
  int src = open(srcFile, O_RDONLY);
  if( src == -1 ){
    perror("Error: open srcFile\n");
    exit(1);
  }

  /* création et ouverture du fichier cible */
  int res = open(resFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if( res == -1 ){
    perror("Error: open resFile\n");
    exit(1);
  }

  /* récupération de la taille du fichier source */
  int size;
  struct stat stt;
  if( fstat(src, &stt) == -1){
    perror("Error: fstat\n");
    exit(1);
  }
  size = stt.st_size;

  /* principe de lecture inversée avec pread :
   * lecture char par char de size-1 à 0.
   */

  char buf;
  int i;
  for( i=size-1; i>=0; i--){
    /* lecture */
    if( pread(src, &buf, sizeof(char), i) == -1){
      perror("Error: pread\n");
      exit(1);
    }
    /* écriture */
    if( write(res, &buf, sizeof(char)) == -1){
      perror("Error: write boucle\n");
      exit(1);
    }
  }
  
  /* fermeture des deux fichiers */
  if( close(src) == -1 || close(res) == -1 ){
    perror("Error: close\n");
    exit(1);
  }

  return EXIT_SUCCESS;
}
