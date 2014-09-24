/**
 * TP1 - Exo5 - Synchronisation de Processus
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
#include <signal.h>
#include <sys/types.h>


/**
 * Execution du processus P3 ( petit-fils )
 * Envoie un SIGUSR2 à son grand-père ( P1 ) et se termine
 *
 * @param pid_t pid_p1 pid du grand père ( P1 )
 */
void p3(pid_t pid_p1)
{
  kill(pid_p1, SIGUSR2);
  exit(EXIT_SUCCESS);
}

/**
 * Traitement du signal SIGUSR2 envoyé par P3
 *
 * @param int sig signal catché 
 */
void p3_handler(int sig)
{
  printf("Processus P3 créé\n");
}

/**
 * Traitements relatifs au processus P2 ( fils )
 * 1- Crée P3
 * 2- Attend la terminaison de P3
 * 3- envoie un SIGUSR1 à P1 puis se termine
 *
 * @param pid_t pid_p1 pid du père ( P1 )
 */
void p2(pid_t pid_p1)
{
  /* création de P3 */
  pid_t pid_p3 = fork();
  if ( pid_p3 == -1 ) {
    perror("fork : error\n");
    exit(EXIT_FAILURE);
  } else if ( !pid_p3 ){
    p3(pid_p1);
  } else {
    /* Attente de la terminaison de P3 */
    wait(NULL);
    
    kill(pid_p1, SIGUSR1);
    exit(EXIT_SUCCESS);
  }
}

/**
 * Traitement du signal envoyé par P2
 *
 * @param int sig signal catché
 */
void p2_handler(int sig)
{
  printf("Processus P3 terminé\n");
}

int main(int argc, char** argv)
{
  pid_t pid_p1 = getpid();
  pid_t pid_p2;

  sigset_t p3_set;
  struct sigaction p3_act;

  sigemptyset(&p3_set);
  p3_act.sa_mask = p3_set;
  p3_act.sa_flags = 0;
  p3_act.sa_handler = p3_handler;

  sigset_t p2_set;
  struct sigaction p2_act;
  
  sigemptyset(&p2_set);
  p2_act.sa_mask = p2_set;
  p2_act.sa_flags = 0;
  p2_act.sa_handler = p2_handler;

  /* installation des handlers */
  sigaction(SIGUSR1, &p2_act, 0);
  sigaction(SIGUSR2, &p3_act, 0);

  /* création de P2 */
  pid_p2 = fork();
  if ( pid_p2 == -1 ) {
    perror("fork : error\n");
    exit(EXIT_FAILURE);
  } else if ( !pid_p2 ) {
    p2(pid_p1);
  } else {
    wait(NULL);
    printf("Processus P2 terminé\n");
  }
  return EXIT_SUCCESS;
}
