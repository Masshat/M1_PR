/**
 * TP1 - Exo7 - Signaux SIGSTOP, SIGCONT, SIGCHLD
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

int comp_main = 0;

void main_proc_sig_handler(int sig)
{
  if ( sig == SIGCHLD && comp_main == 0 ){
    printf("Tous les descendants stoppés\n");
    comp_main++;
  } else if ( sig == SIGCHLD && comp_main == 1 ){
    printf("pid%d Fin du programme\n",getpid());
  }
}

void fils_sig_handler(int sig)
{
  if ( sig == SIGCHLD ){
    printf("pid%d STOP\n",getpid());
    kill(getpid(),SIGSTOP);
  }
}


int main(int argc, char** argv)
{
  /*
   * Remarque : on a pas le droit d'utiliser wait
   */
  
  int i = -1; /* index compteur de processus */
  pid_t pids_array[NB_PROC]; /* tableau de tous les pids de la chaîne */
  pid_t pid_fils; /* pid du fils créé */

  sigset_t sig_proc;
  struct sigaction act;
  
  /* installation des handlers pour le proc principal ( sera désactivé pour les fils ) */
  sigemptyset(&sig_proc);
  act.sa_mask = sig_proc;
  act.sa_handler = main_proc_sig_handler;
  act.sa_flags = 0;
  sigaction(SIGCHLD, &act, NULL);

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
    /* installation du handler fils pour le premier ( sera hérité par les autres ) ... */
    if ( i == 0 ) {
      /* handle du SIGCHLD par la fonction fils_sig_hander */
      act.sa_handler = fils_sig_handler;
      sigaction(SIGCHLD, &act, NULL);
    }
  }
  /* attente & affichages */

  sigset_t sig_proc_2;

  if ( pid_fils ) {
    /* attente : chaque proc est suspendu en attendant un SIGCHLD ( son fils est stoppé ) */
    sigfillset(&sig_proc_2);
    sigdelset(&sig_proc_2, SIGCHLD);
    sigsuspend(&sig_proc_2);
    /* après la reprise du boulot : on annule le handler ( sauf père ) et on relance le fils*/
    if ( getpid() != pids_array[0] ){
      act.sa_handler = SIG_DFL;
      sigaction(SIGCHLD, &act, NULL);
    }
    kill(pid_fils,SIGCONT);
    printf("pid%d CONT\n",getpid());
    /* nouvelle attente de SIGCHLD */
    sigsuspend(&sig_proc_2);
    printf("suspended\n");
    
  } else {
    printf("Dernier fils : affichage de tous les pids :\n");
    for ( i=0; i < NB_PROC; i++ )
      printf("pid[%d]=%d\n",i,pids_array[i]);
    printf("Fin affichage\n");
    kill(getpid(),SIGSTOP);
    printf("Dernier après CONT\n");
  }
  /* mot de fin */
  printf("Proc %d : pid%d, pere%d, fils%d : se termine\n",i,getpid(),getppid(),pid_fils);

  return EXIT_SUCCESS;
}
