/**
 * TP1 - Exo6 - Fonction Kill
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
#include <sys/types.h>
#include <signal.h>

#ifndef NB_PROC
#define NB_PROC 5
#endif

void sigint_handler(int sig)
{
  printf("pid%d : sigint suspended\n",getpid());
}

int main(int argc, char** argv)
{
  /*
   * Remarque : on a pas droit d'utiliser wait et SIGCHLD
   */
  
  int i = -1; /* index compteur de processus */
  pid_t pids_array[NB_PROC]; /* tableau de tous les pids de la chaîne */
  pid_t pid_fils; /* pid du fils créé */

  sigset_t sig_proc;
  struct sigaction act;
  
  /* creation de la chaîne */
  while ( ++i < NB_PROC ) {
    pids_array[i] = getpid();
    pid_fils = fork();
    if ( pid_fils == -1 ){
      perror("fork\n");
      exit(1);
    } else if ( pid_fils ) {
      break;
    }
  }

  /* attente & affichages */
  if ( pid_fils ) {
    /* attente : chaque processus est supendu en attendant un SIGINT de son fils */
    sigemptyset(&sig_proc);
    act.sa_flags = 0;
    act.sa_mask = sig_proc;
    act.sa_handler = sigint_handler;
    sigaction(SIGINT,&act,NULL);
    sigfillset(&sig_proc);
    sigdelset(&sig_proc, SIGINT);
    sigsuspend(&sig_proc);
  } else {
    printf("Dernier fils : affichage de tous les pids :\n");
    for ( i=0; i < NB_PROC; i++ )
      printf("pid[%d]=%d\n",i,pids_array[i]);
    printf("Fin affichage\n");
  }

  /* mot de fin */
  printf("Proc %d : pid%d, pere%d, fils%d : se termine\n",i,getpid(),getppid(),pid_fils);
  if ( getpid() != pids_array[0] )
    kill(getppid(), SIGINT);

  return EXIT_SUCCESS;
}
