#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "philosophers.h"

long global = 0;
sem_t A;

// Note the argument and return types: void*
void* fn(void* args){   
    long local = 0;
    for (int i = 0; i < 50*1000*1000; i++) {
        local++;
        sem_wait(&A);
        global++;
        // if (local % 1*1000*1000 == 0) {
        //     printf("Local: %ld\tGlobal: %ld\n", local, global);
        // }
        sem_post(&A);
    }

    printf("Local: %ld\tGlobal: %ld\n", local, global);

    return NULL;
}

int main(){
    // sem_init(&A,1,1);
    // pthread_t threadHandle_1;
    // pthread_t threadHandle_2;
    // pthread_create(&threadHandle_1, NULL, fn, NULL);
    // pthread_create(&threadHandle_2, NULL, fn, NULL);
    // pthread_join(threadHandle_1, NULL);
    // pthread_join(threadHandle_2, NULL);

    dining_philosophers();

    return 0;
}