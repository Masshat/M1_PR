/**
 * TP1 - Exo3 - Fonction exec
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @copyright (c) 2014, toumlilt
 *
 * @version 1.0
 * @package toumlilt/M1/PR
 */

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
  pid_t pid;
  int j=0;
  int i=0;
  while ( i < 2 ) {
    i++;
    if ((pid = fork()) == -1) {
      perror("fork");
      exit(1);
    }
    else if (pid == 0) j=i;
  }
  if ( j==2 ) {
    /*sleep(2);*/
    /*execl("/bin/sleep","sleep","2",NULL);*/
    char* args[3] = {"sleep","2",NULL};
    execv("/bin/sleep",args);
    printf("sans fils\n");
  } else {
    printf("%d fils \n", (i-j));
    while ( j < i ){
      j++;
      wait(NULL);
    }
  }
  return EXIT_SUCCESS;
}
