/**
 * TP4 - EXO_3 - Serveur de valeurs aléatoires à files mutliples
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
#include <string.h>

#ifndef N
#define N 3
#endif

int MQ[N+1];

struct message {
  long type;
  int value;
} msg, msg_fils;

/**
 * Fonction executée par les fils
 */
int f_fils(int num)
{
  /* génération du nombre de messages attendus */
  srand(getpid());
  int max_msg = (int)(rand() % N) + 1; /* 1 <= max_msg <= N */
  printf("--> Fils %d : va attendre %d messages\n", num, max_msg);

  /* envoie du nombre sur la MQ du père */
  msg.type = num+1; /* chaque fils aura son type, le +1 pour pas avoir type=0 */
  msg.value = max_msg;
  if ( msgsnd(MQ[N], &msg, sizeof(int), 0) == -1){
    fprintf(stdout, "error fils %d : msgsnd\n",num);
    return EXIT_FAILURE;
  }

  /* réception de max_msg messages du père et calcul de leur somme */
  int i, somme=0;
  for(i=0; i<max_msg; i++){
    if ( msgrcv(MQ[num], &msg_fils, sizeof(int), 1L, 0) == -1){
      perror("msgrcv fils\n");
      exit(1);
    }
    printf("Fils %d : a recu la valeur %d\n", num, msg_fils.value);
    somme += msg_fils.value;
  }

  /* affichage de la somme */
  printf("Fils %d : somme des valeurs = %d\n",num,somme);

  return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
  /* key + MQ init */
  int i;
  char *path = "/tmp";
  key_t cle;
  for( i=0; i<=N; i++){
    cle = ftok(path, i);
    MQ[i] = msgget(cle, 0666 | IPC_CREAT);
  }

  /* creation des fils */
  msg.type = 1;
  pid_t pid_son;
  for(i=0; i<N; i++){
    pid_son = fork();
    if ( pid_son == -1 ){
      perror("error : fork\n");
      exit(1);
    } else if ( !pid_son ){
      return f_fils(i);
    }
  }

  /* Le père reçoit sur son MQ[N] le nombre de messages qu'attend le fils */
  int j;
  msg_fils.type = 1;
  for(i=0; i<N; i++){
    if (msgrcv(MQ[N], &msg, sizeof(int), i+1, 0) == -1){
      perror("error : msgrcv\n");
      return EXIT_FAILURE;
    }
    /* il envoie donc ce nombre de messages sur la MQ du fils */
    for (j=0; j<msg.value; j++){
      srand(getpid()+i+j);
      msg_fils.value = (int) ((int)rand() % 100);
      printf("Pere va envoyer %d au fils %d\n",msg_fils.value,i);
      if ( msgsnd(MQ[i], &msg_fils, sizeof(int), 0) == -1){
	perror("error : msgsnd pere\n");
	return EXIT_FAILURE;
      }
    }
  }

  /* attente de la bonne terminaison des fils */
  int status;
  for(i=0; i<N; i++){
    if ( wait(&status) == -1 ){
      perror("error : wait\n");
      return EXIT_FAILURE;
    }
    if( !WIFEXITED(status) ){
      perror("error : WIFEXITED : le proc s'est mal terminé\n");
      return EXIT_FAILURE;
    }
  }

  /* suppression des MQ */
  for ( i=0; i<=N; i++){
    if ( msgctl(MQ[i], IPC_RMID, NULL) == -1 ){
      perror("error : msgctl\n");
      return EXIT_FAILURE;
    }
  }

  /* fin */
  printf("main : se termine\n");
    
  return EXIT_SUCCESS;
}
