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

  /* principe de lecture avec lseek :
   * je me déplace à la fin du fichier
   * je lis le dernier caractère manuellement
   * puis la boucle va à chaque fois faire desc -2 avec lseek
   * lire le caractère et l'écrire
   * jusqu'à la lecture de size chars.
   */
  char buf;
  /* déplacement vers l'avant dernière position */
  if( lseek(src, -1*sizeof(char), SEEK_END) == -1 ){
    perror("Error: lseek\n");
    exit(1);
  }

  /* I/O explicite pour le dernier */
  if( read(src, &buf, sizeof(char)) == -1 ){
    perror("Error: read\n");
    exit(1);
  }
  if( write(res, &buf, sizeof(char)) == -1 ){
    perror("Error: write\n");
    exit(1);
  }

  /* je boucle size-1 fois pour le reste des chars */
  int i;
  for( i=1; i<size; i++){
    if( lseek(src, -2*sizeof(char), SEEK_CUR) == -1){
      perror("Error: lseek boucle\n");
      exit(1);
    }
    if( read(src, &buf, sizeof(char)) == -1){
      perror("Error: read boucle\n");
      exit(1);
    }
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
