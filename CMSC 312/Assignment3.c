/* Suraj Ilavala
 * Assignment 3
 * 04/03/2019
 */

//included packages
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include "semaphore.h"

//max number parameters can be typed in as
#define MAX_PRINT 25
#define MAX_JOBS 30
#define SHMEM 50

//constant variables used throughout to maninpulate loops
int NUM_USERS = 0, NUM_PRINT = 0, GLOBAL_POS = 0, BLOCK_PROC = 0;
sem_t emp, elem, full;//use for semaphores
key_t kQue = 9876, kFull = 8765, kEmp = 7654, kElem = 6543;
//used for signal and hold
int input = 1, i = 0;
int test = 0;
int minimum;

sem_t mute, gate;

void hold(void);
//struct to hold the number of jobs
struct prod{
  int numJob;
  int job[MAX_JOBS];
};

//type def to make struct a type when needed
typedef struct prod _prod;
//hold and signal taken from assignment 2
void hold (void){
        sem_wait(&gate);
        sem_wait(&mute);
        input = input - 1;
        if(input > 0){
                sem_post(&gate);
        }
        sem_post(&mute);

}
void signal (void){
        sem_wait(&mute);
        input = input + 1;
        if (input == 1){
                sem_post(&gate);
        }
        sem_post(&mute);
}

//storing memory for semaphores
void shareMem (void){
  int shmid, shmid1, shmid2, shmid3;
  sem_t *temp;
  struct prod *pQue;
  //creats segments for queue, full, empty, and element
  shmid = shmget(kQue, SHMEM, IPC_CREAT | 0666);
  if(shmid < 0){
    perror("shmget");
    exit(1);
  }
  shmid1 = shmget(kFull, SHMEM, IPC_CREAT | 0666);
  if(shmid1 < 0){
    perror("shmget");
    exit(1);
  }
  shmid2 = shmget(kEmp, SHMEM, IPC_CREAT | 0666);
  if(shmid2 < 0){
    perror("shmget");
    exit(1);
  }
  shmid3 = shmget(kElem, SHMEM, IPC_CREAT | 0666);
  if(shmid3 < 0){
    perror("shmget");
    exit(1);
  }
//attachs queue
  pQue = (_prod *) shmat(shmid, 0, 0);
  if(pQue == ((_prod *)(-1))){
    perror("shmget");
    exit(1);
  }
//makes array
  for(i = 0; i < NUM_USERS; i++){
    pQue[i].numJob = 0;
    int j;
    for(j = 0; j < MAX_JOBS; j++){
      pQue[i].job[j] = 0;
    }
  }
  //attachs the full, empty, element
  temp = (sem_t *) shmat(shmid1, 0, 0);
  if(temp == (sem_t *)(-1)){
    perror("shmget");
    exit(1);
  }
  temp = (sem_t *)(sem_init(&full, 1, 0));

  temp = (sem_t *) shmat(shmid2, 0, 0);
  if(temp == (sem_t *)(-1)){
    perror("shmget");
    exit(1);
  }
  temp = (sem_t *)(sem_init(&emp, 1, NUM_USERS));

  temp = (sem_t *) shmat(shmid3, 0, 0);
  if(temp == (sem_t *)(-1)){
    perror("shmget");
    exit(1);
  }
  temp = (sem_t *)(sem_init(&elem, 1, 1));
}
//continues storing and prints
void *CHILD_PARENT (void *args){
  struct prod *pQue;
  int pos = (int)args;
  int thrid = pthread_self();
  int nJob;
  sem_t *t1, *t2, *t3;
  sem_t *constant = (sem_t *)(-1);
  int shmid, shmid1, shmid2, shmid3;
  //creates segment and attachs
  shmid = shmget(kQue, SHMEM, IPC_CREAT | 0666);
  if(shmid < 0){
    perror("shmget");
    exit(1);
  }
  pQue = (_prod *) shmat(shmid, 0, 0);
  if(pQue == ((_prod *)(-1))){
    perror("shmat");
    exit(1);
  }
 //creates segment for full and attachs to t1
  shmid1 = shmget(kFull, SHMEM, IPC_CREAT | 0666);
  if(shmid1 < 0){
    perror("shmget");
    exit(1);
  }

  t1 = (sem_t *) shmat(shmid1, 0, 0);
  if(t1 == constant){
    perror("shmat");
    exit(1);
  }
 //creates segment for empty and attachs to t2
  shmid2 = shmget(kEmp, SHMEM, IPC_CREAT | 0666);
  if(shmid2 < 0){
    perror("shmget");
    exit(1);
  }

  t2 = (sem_t *) shmat(shmid2, 0, 0);
  if(t2 == constant){
    perror("shmat");
    exit(1);
  }
 //creates segment for a element and attachs to t3
  shmid3 = shmget(kElem, SHMEM, IPC_CREAT | 0666);
  if(shmid3 < 0){
    perror("shmget");
    exit(1);
  }

  t3 = (sem_t *) shmat(shmid3, 0, 0);
  if(t3 == constant){
    perror("shmat");
    exit(1);
  }
//stores the data in a temp array to print later
  nJob = pQue[pos].numJob;
  int tjobs[nJob];
  int i;
  for(i = 0; i < nJob; i++){
    tjobs[i] = pQue[pos].job[i];
  }
  //signal();
  //hold();
  sem_wait(&elem);
  sem_wait(&emp);
  pQue[pos].numJob = 0;
  for(i = 0; i < nJob; i++){
    pQue[pos].job[i] = 0;
  }
  //signal();
  sem_post(&elem);
  sem_post(&emp);
//prints child and its ids
  printf("\nId [%u] requests position [%d] and has [%d] jobs\n", thrid, pos + 1, nJob);
  for(i = 0; i < nJob; i++){
    printf("Job id number [%d] has the size of [%d] bytes\n", i, tjobs[i]);
  }
  sleep(1);
  pthread_exit(NULL);
  return NULL;
}
//main method takes in to parameters
int main(int argc, char **argv){
  //variables used throughout main
  int pid, pJobs = 0, jSize = 0;
  int count, count2, tCount, fCount;
  int shmid, shmid1, shmid2, shmid3;
  int tArray[MAX_JOBS];
  double param, param2;
  sem_t *temp, *temp2, *temp3;
  sem_t *constant = (sem_t *)(-1);
  struct prod *ptr, *after;
  //makes sure the input is not below 1 but if it does it adds on the negative
  param = argc > 1 ? atoi(argv[1]) : argc - argc;
  param2 = argc > 1 ? atoi(argv[2]) : argc - argc;


  NUM_USERS = (int) param;
  NUM_PRINT = (int) param2;
  if((NUM_PRINT <= 0) && (NUM_USERS <= 0)){
    printf("Incorrect parameters were given or no parameters were given at all\n");
    printf("Please run again with correct parameters\n");
    return 1;
  }
  pthread_t priThr[NUM_PRINT];
  int idThread[NUM_PRINT];
  shareMem();
  GLOBAL_POS = 0;
  for(fCount = 0; fCount < NUM_USERS; fCount++){
    pJobs = rand() % (30);
    for(count = 0; count < pJobs; count++){
      jSize = rand() % (1000 - 100) + 100;
      tArray[count] = jSize;
    }
    //forks for child process
    pid = fork();
    if(pid < 0){
      printf("Error");
      exit(1);
    }
    else if(pid == 0){
      //if child isnt there creates segments and attachs them
      shmid = shmget(kQue, SHMEM, IPC_CREAT | 0666);
      if(shmid < 0){
        perror("shmget");
        exit(1);
      }
      ptr = (_prod *) shmat(shmid, 0, 0);
      if(ptr == (_prod *)(-1)){
        perror("shmat");
        exit(1);
      }
      shmid1 = shmget(kEmp, SHMEM, IPC_CREAT | 0666);
      if(shmid1 < 0){
        perror("shmget");
        exit(1);
      }
      temp = (sem_t *) shmat(shmid1, 0, 0);
      if(temp == constant){
        perror("shmat");
        exit(1);
      }
      shmid2 = shmget(kFull, SHMEM, IPC_CREAT | 0666);
      if(shmid1 < 0){
        perror("shmget");
        exit(1);
      }
      temp2 = (sem_t *) shmat(shmid2, 0, 0);
      if(temp2 == constant){
        perror("shmat");
        exit(1);
      }
      shmid3 = shmget(kElem, SHMEM, IPC_CREAT | 0666);
      if(shmid3 < 0){
        perror("shmget");
        exit(1);
      }
      temp3 = (sem_t *) shmat(shmid3, 0, 0);
      if(temp3 == constant){
        perror("shmat");
        exit(1);
      }
      //signal();
      //hold();
      sem_wait(&elem);
      sem_wait(&emp);
      ptr[fCount].numJob = pJobs;
      for(count = 0; count < pJobs; count++){
        ptr[fCount].job[count] = tArray[count];
      }
      printf("Id [%d] form parent [%d] add [%d] requests and has [%d] jobs available\n", getpid(), getppid(), fCount + 1, pJobs);
      //signal();
      sem_post(&elem);
      sem_post(&full);
      exit(0);
    }
    else{
      wait(NULL);
    }
  }
  //this makes sure position doesnt pass users and the max number of prints
    for(tCount = 0; tCount < NUM_PRINT; tCount++){
      if((GLOBAL_POS < NUM_USERS) && (GLOBAL_POS < MAX_PRINT)){
        idThread[tCount] = tCount;
        pthread_create(&priThr[tCount],NULL,CHILD_PARENT,GLOBAL_POS);
        pthread_join(priThr[tCount], NULL);
        GLOBAL_POS++;
      }
    }
  exit(0);
}
