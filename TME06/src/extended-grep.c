/**
 * TP6 - EXO_6 - Fonction grep étendue
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
#include <dirent.h>
#include <fcntl.h>

#define BUFF_SIZE 128

int main (int argc, char* argv [])
{
  /* vérification du nombre d'arguments */
  if( argc != 3 ){
    perror("Usage: extended-grep <expr> <chemin>\n");
    return EXIT_FAILURE;
  }

  /* init */
  char* dirPath = argv[2];
  char* exp = argv[1];

  /* ouverture du répertoire */
  DIR *pt_Dir = opendir(dirPath);
  if( !pt_Dir ){
    perror("Error: opendir\n");
    return EXIT_FAILURE;
  }
	
  /* lecture des fichiers du répertoire */
  struct dirent* dirEnt;
  struct stat buf;
  int fd, nb_lu;
  int flag=0; /* 0 tant qu'aucune chaîne n'a été trouvée, 1 sinon */
  char filepath[BUFF_SIZE]; /* où sera construit le chemin vers le fichier */
  char buffer[BUFF_SIZE]; /* pour la lecture */
  
  while ( (dirEnt = readdir(pt_Dir)) ) {
    /* on ne prend pas le . , .. , et les fichiers cachés */
    if( dirEnt->d_name[0] == '.' )
      continue;
    
    /* construction du filepath */
    memset(filepath, '\0', BUFF_SIZE);
    strcat(filepath, dirPath);
    strcat(filepath, "/");
    strcat(filepath, dirEnt->d_name);

    /* consultation de l'i-node, vérification de régularité du fichier */
    if( stat(filepath, &buf) == -1){
      perror("Error: stat\n");
      exit(1);
    }
    if( !S_ISREG(buf.st_mode) ){
      continue; /* si non régulier on passe au suivant ... */
    }

    /* ouverture du fichier */
    fd = open(filepath, O_RDONLY);
    if( fd == -1 ){
      perror("Error: open\n");
      exit(1);
    }

    /* lecture du fichier */
    while( (nb_lu=read(fd, buffer, BUFF_SIZE)) ){
      /* si une occurence de la chaîne est présente */
      if( strstr(buffer, exp) ){
	flag = 1;
	printf("%s\n", dirEnt->d_name);
	break;
      }
    }

    /* fermeture du fichier */
    if( close(fd) == -1 ){
      perror("Error: close\n");
      exit(1);
    }
  }

  /* fermeture du répertoire */
  if( closedir(pt_Dir) == -1 ){
    perror("Error: closedir\n");
    exit(1);
  }

  /* si aucune occurence de la chaîne n'a été trouvée */
  if( !flag ){
    printf("Aucun fichier valide\n");
  }
	
  return EXIT_SUCCESS;
}
