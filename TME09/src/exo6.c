/**
 * TP7 - EXO6
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

int nb_rec = 0;
int sum=0;

void interrupt_signal(int signo, siginfo_t *si, void *context)
{         
  printf("ASSOCIATED VALUE - [%d]\n",si->si_value.sival_int);
  nb_rec++;
  sum += si->si_value.sival_int;
}

int main()
{
  sigset_t block_mask;
  struct sigaction action;

  action.sa_sigaction = interrupt_signal; 
  action.sa_flags = SA_SIGINFO; 
  sigfillset(&block_mask); 
  sigdelset(&block_mask,SIGRTMIN); 
  action.sa_mask = block_mask; 
  sigaction(SIGRTMIN, &action, 0); 
  sigprocmask(SIG_SETMASK, &block_mask, 0);

  pid_t pidtab[NB_PROCS];

  union sigval val;
  int mysig; 
  int i;

  for(i = 0 ; i < NB_PROCS ; i++ )
    {
      switch(pidtab[i] = fork())
	{
	case -1:
	  printf(FAILED);
	  perror("ERROR create proc");
	  exit(0);

	case 0:
	  srand(time(NULL) ^ (getpid()<<16));
	  val.sival_int = ((int)((rand()%RAND_MAXI)));
#ifdef DEBUG
	  printf("[MAIN SON] sigqueue   => ");
#endif
	  if (sigqueue(getppid(), SIGRTMIN, val) == -1) {printf(FAILED);perror("sigqueue");exit(1);}
#ifdef DEBUG
	  printf(OK);
#endif
	  return 0;
	default:
	  break;
	}
    }

  while(1)
    {
#ifdef DEBUG
      printf("[MAIN FATHER] sigwait => ");
#endif
      if (sigwait(&block_mask, &mysig) == -1) {printf(FAILED);perror("sigwait");exit(1);}
#ifdef DEBUG
      printf(OK);
#endif
      if(nb_rec >= NB_PROCS)
	break;
    }
  
#ifdef SUM
  printf("-----------------------\n[FATHER] sum     - [%d]\n",sum);
#endif

  return EXIT_SUCCESS;
}
