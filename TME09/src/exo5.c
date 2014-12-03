/**
 * TP7 - EXO5
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

void interrupt_signal(int signo, siginfo_t *si, void *context){}

int main()
{
  sigset_t block_mask;
  struct sigaction action;

  action.sa_sigaction = interrupt_signal; 
  action.sa_flags = SA_SIGINFO;
  sigfillset(&block_mask); 

  action.sa_mask = block_mask; 
  sigaction(SIGRTMIN, &action, 0); 
  sigprocmask(SIG_SETMASK, &block_mask, 0);
  
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
	  if(i == NB_PROCS -1)
	    {
	      printf("PROCESS [%d] => passage [%d]\n",getpid() , i);
	      kill(pidtab[i-1],SIGRTMIN);
	    }
	  else
	    {
	      sigdelset(&block_mask,SIGRTMIN);
	      sigsuspend(&block_mask);
	      printf("PROCESS [%d] => passage [%d]\n",getpid() , i);
	    }
	  if(i > 0)
	    kill(pidtab[i-1],SIGRTMIN);
	  return 0;
	default:
	  break;
	}
    }

  /*Wait sons process*/
  for(i = 0 ; i < NB_PROCS ; i++)
    wait(0);
  
  return EXIT_SUCCESS;
}
