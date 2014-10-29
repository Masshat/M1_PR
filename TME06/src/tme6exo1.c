/**
 * Fichier fourni
 */

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


int main (void) {
  int fd1, fd2, fd3;
  if ((fd1 = open ("src/testfiles/fich1", O_RDWR| O_CREAT | O_TRUNC,0600)) == -1)
    return EXIT_FAILURE;
  if (write (fd1,"abcde", strlen ("abcde")) == -1) /* A */
    return EXIT_FAILURE;
  if (fork () == 0) {
    if ((fd2 = open ("src/testfiles/fich1", O_RDWR)) == -1)
      return EXIT_FAILURE;
    if (write (fd1,"123", strlen ("123")) == -1) /* B */
      return EXIT_FAILURE;
    if (write (fd2,"45", strlen ("45")) == -1) /* C */
      return EXIT_FAILURE;
    close(fd2);
  } else {
    fd3 = dup(fd1);
    if (lseek (fd3,0,SEEK_SET) == -1) /* D */
      return EXIT_FAILURE;
    if (write (fd3,"fg", strlen ("fg")) == -1) /* E */
      return EXIT_FAILURE;
    if (write (fd1,"hi", strlen ("hi")) == -1) /* F */
      return EXIT_FAILURE;
    wait (NULL);
    close (fd1);
    close(fd3);
  }
  return EXIT_SUCCESS;
}
