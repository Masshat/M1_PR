/**
 * TP4 - EXO_1 - Remontée de valeurs par file de messages
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @copyright (c) 2014, toumlilt
 *
 * @version 1.0
 * @package toumlilt/M1/PR
 */

#define _SVID_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#ifndef N_FILS
#define N_FILS 5
#endif

int msg_id;
struct message {
  long type;
  int random_val;
} msg;

/**
 * Fonction executée par les fils 
 */
int f_fils(int num)
{
  srand(getpid());
  int random_val = (int)(rand() % 10);
  printf("fils %d : generated random_val = %d\n", num, random_val);
  
  msg.random_val = random_val;
  if ( msgsnd(msg_id, &msg, sizeof(int), 0) == -1){
    perror("error : msgsnd\n");
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
  /* init key */
  char *path = "/tmp";
  char code = 'C';
  key_t cle = ftok(path, code);

  /* création de la file de messages */
  msg_id = msgget(cle, 0666 | IPC_CREAT);
  msg.type = 1;

  /* création des fils */
  pid_t pid_son;
  int i;
  for(i=0; i<N_FILS; i++){
    pid_son = fork();
    if ( pid_son == -1 ){
      perror("error : fork\n");
      exit(1);
    } else if ( !pid_son ){
      return f_fils(i);
    }
  }

  /* attente des fils */
  int status;
  for(i=0; i<N_FILS; i++){
    if ( wait(&status) == -1 ){
      perror("error : wait\n");
      return EXIT_FAILURE;
    }
    if( !WIFEXITED(status) ){
      perror("error : WIFEXITED : le proc s'est mal terminé\n");
      return EXIT_FAILURE;
    }
  }

  /* extraction des messages et calcul de la somme */
  int somme = 0;
  for(i=0; i<N_FILS; i++){
    if (msgrcv(msg_id, &msg, sizeof(int), 1L, 0) == -1 ){
      perror("error : msgrcv\n");
      return EXIT_FAILURE;
    }
    somme += msg.random_val;
  }

  /* suppression de la file */
  if ( msgctl(msg_id, IPC_RMID, NULL) == -1 ){
    perror("error : msgctl\n");
    return EXIT_FAILURE;
  }

  printf("main : somme = %d\n",somme);
    
  return EXIT_SUCCESS;
}
