/**
 * TP7 - EXO3
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
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <aio.h>
#include <time.h>

#include "color.h"

#define NB_PROCS 5
#define RAND_MAXI 10

int nb_lect = 0;
int nb_not = 0;
int fd;
int fd2;
int ran;
union sigval val;

int tablect[NB_PROCS];

sigset_t mask;

void notify_father()
{
  val.sival_int = ran;
  #ifdef DEBUG
  printf("[NOTIFY_FATHER] sigqueue => ");
  #endif
  if (sigqueue(getppid(), SIGRTMIN, val) == -1) {printf(FAILED);perror("sigqueue");exit(1);}
  #ifdef DEBUG
  printf(OK);
  #endif
}

void read_father()
{
  struct aiocb a;
  a.aio_fildes = fd2;
  
  a.aio_buf = &tablect[nb_lect];
  a.aio_nbytes = sizeof(int);
  a.aio_offset = nb_lect*sizeof(int);
  a.aio_reqprio = 0;
  a.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  a.aio_sigevent.sigev_signo = SIGRTMIN + 1;
  nb_lect++;
  aio_read(&a);
  puts("/!\\ NOT YET, WAIT /!\\");
  sigsuspend(&mask);
}

void print_father()
{
  puts("FATHER");
}

int main(int argc, char **argv)
{
  if(argc != 2) {printf(FAILED);perror("NUMBER OF ARGUMENTS");exit(0);}
  
  int mysig;
  sigset_t block_mask;
  struct sigaction action;
  
  action.sa_sigaction = print_father; 
  action.sa_flags = SA_SIGINFO;
  sigfillset(&block_mask); 
  sigdelset(&block_mask, SIGINT);
  
  action.sa_mask = block_mask; 
  sigaction(SIGRTMIN + 1, &action, 0);
  sigprocmask(SIG_SETMASK, &block_mask, 0);

  #ifdef DEBUG
  printf("[MAIN] open fd           => ");
  #endif
  if ((fd = open(argv[1],O_CREAT | O_TRUNC | O_RDWR | O_ASYNC,0666)) == -1) {printf(FAILED);perror("ERROR open fd");exit(0);}
  #ifdef DEBUG
  printf(OK);
  #endif

  pid_t pidtab[NB_PROCS];
  int i;

  for(i = 0 ; i < NB_PROCS ; i++ )
    {
      switch(pidtab[i] = fork())
	{
	case -1:
	  printf(FAILED);
	  perror("ERROR create process");
	  exit(0);
	  
	case 0:
	  sigfillset(&mask);
	  sigdelset(&mask,SIGRTMIN);
	  
	  action.sa_sigaction = notify_father; 
	  sigaction(SIGRTMIN, &action, 0); 
	  
	  srand(time(NULL) ^ (getpid()<<16));
	  ran = ((int)((rand()%RAND_MAXI)));
	  printf("RAN[%d] = %d\n",i,ran);
	  
	  struct aiocb a;
	  a.aio_fildes = fd;
	  a.aio_buf = &ran;
	  a.aio_nbytes = sizeof(int);
	  a.aio_offset = i*sizeof(int);
	  a.aio_reqprio = 0;
	  a.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
	  a.aio_sigevent.sigev_signo = SIGRTMIN;
	  aio_write(&a);
	  
	  sigsuspend(&mask);
	  return 0;
	default:
	  break;
	}
    }
  
  #ifdef DEBUG
  printf("[MAIN] open fd           => ");
  #endif
  if ((fd2 = open(argv[1], O_RDWR | O_ASYNC,0666)) == -1) {printf(FAILED);perror("ERROR open fd2");exit(0);}
  #ifdef DEBUG
  printf(OK);
  #endif
  
  sigemptyset(&mask);
  sigaddset(&mask,SIGRTMIN);
  
  while(1)
    {
      if (sigwait(&mask, &mysig) == -1) {printf(FAILED);perror("sigwait");exit(1);}
      if(mysig == SIGRTMIN)
	read_father();
      if(nb_lect >= NB_PROCS)
	break;
    }
  
  sigfillset(&mask);
  sigdelset(&mask,SIGRTMIN+1);
  sigdelset(&mask,SIGINT);

  for(i=0 ; i< NB_PROCS ; i++)
    printf("SON[%d] => %d\n",i,tablect[i]);

  close(fd);
  close(fd2);

  return EXIT_SUCCESS;
}
