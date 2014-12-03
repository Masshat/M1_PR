/**
 * TP7 - EXO4
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

#include "color.h"

#define DISPLACEMENT 10

void interrupt_signal(int signo, siginfo_t *si, void *context)
{
  puts("Sending Signal");
}

int main(int argc, char **argv)
{
  if(argc != 3) {printf(FAILED);perror("Source File and Target File REQUIRED");exit(0);}

  sigset_t block_mask;
  struct sigaction action;

  action.sa_sigaction = interrupt_signal; 
  action.sa_flags = SA_SIGINFO;
  sigfillset(&block_mask); 

  action.sa_mask = block_mask; 
  sigaction(SIGRTMIN, &action, 0); 
  sigprocmask(SIG_SETMASK, &block_mask, 0);

  int fd1,fd2;
  char buffer[10];
  int reading = DISPLACEMENT;
  int num_reading = 0;

  int i;
  struct aiocb *list[10];
  
  for(i=0;i<10;i++)
    list[i] = malloc(sizeof(struct aiocb));

  struct sigevent sig;
  sig.sigev_notify = SIGEV_SIGNAL;
  sig.sigev_signo = SIGRTMIN;

#ifdef DEBUG
  printf("[MAIN] open fd1       => ");
#endif
  
  if((fd1 = open(argv[1], O_RDONLY,0600))==-1) {printf(FAILED);perror("OPEN Source");exit(1);}
  
#ifdef DEBUG
  printf(OK);
  printf("[MAIN] open fd2       => ");
#endif
  
  if((fd2 = open(argv[2], O_RDWR | O_CREAT | O_TRUNC | O_ASYNC,0600))==-1) {printf(FAILED);perror("OPEN Target");exit(1);}
  
#ifdef DEBUG
  printf(OK);
#endif

  for(i = 0 ; i < 10 ; i++)
    {
      list[i]->aio_fildes = fd2;
      list[i]->aio_buf = &buffer[i];
      list[i]->aio_nbytes = sizeof(char);
      /*list.aio_offset = nb_lect*sizeof(int);*/
      list[i]->aio_reqprio = 0;
      list[i]->aio_sigevent.sigev_notify = SIGEV_SIGNAL;
      list[i]->aio_sigevent.sigev_signo = SIGRTMIN;
      list[i]->aio_lio_opcode = LIO_WRITE;
    }
  
  off_t offset;
#ifdef DEBUG
  printf("[MAIN] lseek          => ");
#endif
  if ((offset = lseek (fd1, -(DISPLACEMENT + 1), SEEK_END)) < 0) {printf(FAILED);perror("lseek");exit(0);}
#ifdef DEBUG
  printf(OK);
#endif

  while(offset >= 0)
    {
      pread (fd1, buffer, reading, offset);
      buffer[10] = '\0';
      printf("buffer = %s\n",buffer);
      
      if(offset - DISPLACEMENT < 0)
	{
	  for(i = 0 ; i < 10 ; i++)
	    list[i]->aio_offset = num_reading * DISPLACEMENT + i;
	  
#ifdef DEBUG
	  printf("[MAIN] lio_listio I   => ");
#endif
	  if(lio_listio(LIO_WAIT, list, 10, &sig) == -1) {printf(FAILED);perror("error in the chain of writings");exit(0);}
#ifdef DEBUG
	  printf(OK);
#endif
	  num_reading++;
	  
	  reading = offset;
	  offset = 0;
	  pread (fd1, buffer, reading, offset);
	  buffer[reading] = '\0';
	  printf("buffer = %s\n",buffer);
	  
	  for(i = 0 ; i < 10 ; i++)
	    list[i]->aio_offset = num_reading * DISPLACEMENT + i;
	  
#ifdef DEBUG
	  printf("[MAIN] lio_listio II  => ");
#endif
	  if(lio_listio(LIO_WAIT, list, reading, &sig) == -1) {printf(FAILED);perror("error in the chain of writings");exit(0);}
#ifdef DEBUG
	  printf(OK);
#endif
	  break;
	}
      offset = lseek (fd1, -DISPLACEMENT, SEEK_CUR);
      
      for(i = 0 ; i < 10 ; i++)
	list[i]->aio_offset = num_reading * DISPLACEMENT + i;
    
#ifdef DEBUG
      printf("[MAIN] lio_listio III => ");
#endif
      if(lio_listio(LIO_WAIT, list, 10, &sig) == -1) {printf(FAILED);perror("error in the chain of writings");exit(0);}
#ifdef DEBUG
      printf(OK);
#endif
      
      num_reading++;
    }
  
  sigdelset(&block_mask, SIGRTMIN);
  
  for(i=0;i<=num_reading;i++)
    sigsuspend(&block_mask);
  
  close(fd1);
  close(fd2);
  return EXIT_SUCCESS;
}
