/**
 * TP7 - EXO1
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @copyright (c) 2014, toumlilt
 *
 * @version 1.0
 * @package toumlilt/M1/PR
 */

#define _XOPEN_SOURCE 700

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <aio.h>
#include <fcntl.h>

void interrupt_signal(int signo, siginfo_t *si, void *context)
{
  puts("sending signal");
}

int main(int argc, char **argv)
{
  sigset_t block_mask;
  struct sigaction action;

  action.sa_sigaction = interrupt_signal; 
  action.sa_flags = SA_SIGINFO;
  sigfillset(&block_mask); 

  action.sa_mask = block_mask; 
  sigaction(SIGRTMIN, &action, 0); 
  sigprocmask(SIG_SETMASK, &block_mask, 0);


  if(argc != 3)
    {
      printf("USE: %s <file name> <character string>\n",argv[0]);
      return EXIT_FAILURE;
    }

  if(strlen(argv[2])>20)
    {
      puts("Your characters string must be more tiny !!!\n");
      exit(0);
    }

  int fd = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY | O_ASYNC, 0600);

  struct aiocb a;
  a.aio_fildes = fd;
  a.aio_buf = argv[2];
  a.aio_nbytes = strlen(argv[2]);
  a.aio_offset = 0;
  a.aio_reqprio = 0;
  a.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  a.aio_sigevent.sigev_signo = SIGRTMIN;

  aio_write(&a);

  int fd2 = open(argv[1] , O_RDWR,0600);
  char buffer[20];
  
  sigdelset(&block_mask,SIGRTMIN);
  sigsuspend(&block_mask);

  printf("[READ] string in %s\n",argv[1]);
  read(fd2,buffer,20);

  printf("buffer = %s\n",buffer);

  close(fd);

  return EXIT_SUCCESS;
}
