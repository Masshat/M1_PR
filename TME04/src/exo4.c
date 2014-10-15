/**
 * TP4 - EXO_4 - Serveur de valeurs aléatoires à file unique
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

/* Principe d'utilisation de la file unique :
 * Les fils déposeront leurs messages pour le père dans type=num_fils+1
 * Le père déposera ensuite les valeurs pour chaque fils dans
 * type=num_fils+1+N
 */
int MQ;

struct message {
  long type;
  int value;
} msg, msg_fils;

/**
 * Fonction executée par les fils
 */
int f_fils(int num)
{
  /* init du nombre de messages attendus max_msg dans [1,N] */
  srand(getpid());
  int max_msg = (int)(rand() % N) + 1; /* 1 <= max_msg <= N */
  printf("--> fils %d : va attendre %d messages\n", num, max_msg);

  /* envoie du nombre de messages attendus au père */
  msg.type = num+1;
  msg.value = max_msg;
  if ( msgsnd(MQ, &msg, sizeof(int), 0) == -1){
    fprintf(stdout, "error fils %d : msgsnd\n",num);
    return EXIT_FAILURE;
  }

  /* réception des valeurs et calcul de la somme*/
  int i, somme=0;
  for(i=0; i<max_msg; i++){
    if ( msgrcv(MQ, &msg_fils, sizeof(int), num+1+N, 0) == -1){
      perror("msgrcv fils\n");
      exit(1);
    }
    printf("Fils %d : a recu la valeur %d\n", num, msg_fils.value);
    somme += msg_fils.value;
  }

  /* affichage de la somme calculée */
  printf("Fils %d : somme des valeurs = %d\n",num,somme);

  return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
  /* Init key */
  int i,j;
  char *path = "/tmp";
  key_t cle = ftok(path, 7);

  /* création de la file de messages */
  MQ = msgget(cle, 0666 | IPC_CREAT);

  /* création des fils */
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

  /* réception du nombre de messages attendus par chaque fils */
  for(i=0; i<N; i++){
    if (msgrcv(MQ, &msg, sizeof(int), i+1, 0) == -1){
      perror("error : msgrcv\n");
      return EXIT_FAILURE;
    }
    /* calcul des valeurs et envoie au fils numéro i */
    msg_fils.type=i+1+N;
    for (j=0; j<msg.value; j++){
      srand(getpid()+i+j);
      msg_fils.value = (int) ((int)rand() % 100);
      printf("Pere va envoyer %d au fils %d\n",msg_fils.value,i);
      if ( msgsnd(MQ, &msg_fils, sizeof(int), 0) == -1){
	perror("error : msgsnd pere\n");
	return EXIT_FAILURE;
      }
    }
  }
  
  /* attente de bonne terminaison des fils */
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

  /* supression de la file de messages */
  if ( msgctl(MQ, IPC_RMID, NULL) == -1 ){
    perror("error : msgctl\n");
    return EXIT_FAILURE;
  }
  
  /* fin */
  printf("main : se termine\n");
    
  return EXIT_SUCCESS;
}
