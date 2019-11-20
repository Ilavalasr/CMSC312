#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#define MINIMUM(x,y) ((x<y) ? x:y)
int input = 1;

int test = 0;
int minimum;
sem_t mute, gate;

void hold (void);
void signal (void);

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
void* CSem(void * a){
       sem_getvalue(&mute, &test);
       hold();
       printf("Value is : %d, Mutex is: %d\n", input, test);
       sleep(1);
       signal();
}
int main(int argc, char*argv[]){
       pthread_t thread1, thread2, thread3;
       minimum = MINIMUM(1, input);
       sem_init(&gate, 0, minimum);
       sem_init(&mute, 0, 1);

       if(pthread_create(&thread1, NULL, CSem, NULL)){
               printf("Error thread 1\n");
               exit(1);
       }

        if(pthread_create(&thread2, NULL, CSem, NULL)){
                printf("Error thread 2\n");
                exit(1);
        }
        if(pthread_create(&thread3, NULL, CSem, NULL)){
                printf("Error thread 3\n");
                exit(1);
        }
        if(pthread_join(thread1, NULL)){
                printf("Error join thread 1\n");
                exit(1);
        }
        if(pthread_join(thread2, NULL)){
                printf("Error join thread 2\n");
                exit(1);
        }
        if(pthread_join(thread3, NULL)){
                printf("Error join thread 3\n");
                exit(1);
        }
        pthread_exit(NULL);

}
