/**
 * TP7 - EXO7
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @copyright (c) 2014, toumlilt
 *
 * @version 1.0
 * @package toumlilt/M1/PR
 */

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/un.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <strings.h>
#include <signal.h>

# define NB 10

int mysig, i, pere, pid[NB]; 
union sigval val;
sigset_t sig;
struct sigaction action;
int somme = 0;

void handler(int signo, siginfo_t *si, void *context)
{
  somme = somme + (int)((si->si_value).sival_int);
  if (somme >= NB)
    {
      for( i=0; i <NB; i++)
	{
	  if( sigqueue(pid[i], SIGRTMAX, val) < 0) {perror("sigqueue handler process");exit(1);}
	}
    }
}

void handler2(int signo, siginfo_t *si, void *context)
{
  printf("apres barriere\n");
  exit(0);
}

void wait_barrier(int nb)
{
  sigfillset(&sig);
  sigprocmask(SIG_UNBLOCK, &sig, NULL);
  sigemptyset(&sig);
  sigaddset(&sig, SIGRTMAX);
  
  if( sigwait(&sig, &mysig) < 0)
    {
      perror("sigwait main");
      exit(1);
    }
}

void process(int nb)
{
  printf("avant barriere\n");
  val.sival_int = 1;
  
  sigfillset(&sig);
  sigprocmask(SIG_BLOCK, &sig, NULL);
  
  action.sa_sigaction = handler2;
  action.sa_flags = SA_SIGINFO;
  action.sa_mask = sig;
  sigaction(SIGRTMAX, &action, 0);

  if( sigqueue(pere, SIGRTMIN, val) < 0) {perror("sigqueue process");exit(1);}
  wait_barrier(NB);
}

int main( int argc, char **argv)
{
  pere = getpid();
  sigfillset(&sig);
  sigprocmask(SIG_BLOCK, &sig, NULL);
  int tmp;

  for( i=0; ((i<NB )& ((tmp = fork()) != 0) && (pere ==getpid())); i++)
    {
      pid[i] = tmp;
    }

  if (pere == getpid())
    {
      action.sa_sigaction = handler;
      action.sa_flags = SA_SIGINFO;
      action.sa_mask = sig;
      sigaction(SIGRTMIN, &action, 0);
      
      sigfillset(&sig);
      sigprocmask(SIG_UNBLOCK, &sig, NULL);
      sigemptyset(&sig);
      sigaddset(&sig, SIGRTMIN);
      
      if (sigwait(&sig, &mysig) < 0) {perror("sigwait main");exit(1);}
      printf("--------------------------\n");
      for( i=0; i<NB; i++)
	{
	  wait(0);
	}
      printf("pere se termine\n");
    }
  else
    process(NB);
  
  return EXIT_SUCCESS;
}
