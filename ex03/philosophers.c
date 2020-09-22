#include "philosophers.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

static inline void think(void){
    for(long i = 0; i < 100000000; i++){
        // "Memory clobber" - tells the compiler optimizer that all the memory 
        // is being touched, and that therefore the loop cannot be optimized out
        asm volatile("" ::: "memory");
    }
}

pthread_mutex_t forks[5];

struct args {
    int philosopher;
};

void* eat(void* input) {
    int philosopher = ((struct args*)input)->philosopher;
    
    if (philosopher == 0) {
        pthread_mutex_lock(&forks[(philosopher + 1) % 5]); //lock right
        printf("T%d picks up fork #%d\n", philosopher, (philosopher + 1) % 5);
        think();
        pthread_mutex_lock(&forks[philosopher]); //lock left
        printf("T%d picks up fork #%d\n", philosopher, philosopher);
        think();
        pthread_mutex_unlock(&forks[(philosopher + 1) % 5]); //unlock right
        printf("T%d puts down fork #%d\n", philosopher, (philosopher + 1) % 5);
        pthread_mutex_unlock(&forks[philosopher]); //unlock left
        printf("T%d puts down fork #%d\n", philosopher, philosopher);
    } else {
        pthread_mutex_lock(&forks[philosopher]); //lock left
        pthread_mutex_lock(&forks[(philosopher + 1) % 5]); //lock right
        think();
        pthread_mutex_unlock(&forks[philosopher]); //unlock left
        pthread_mutex_unlock(&forks[(philosopher + 1) % 5]); //unlock right
    }
}

void dining_philosophers() {

    pthread_t philosophers[5];
    for (int fork = 0; fork < 5; fork++) {
        pthread_mutex_init(&forks[fork], NULL);
    }
    for (int philosopher = 0; philosopher < 5; philosopher++) {
        struct args *input = (struct args *)malloc(sizeof(struct args));
        input->philosopher = philosopher;
        pthread_create(&philosophers[philosopher], NULL, eat, (void*)&philosopher);
    }
    for (int philosopher = 0; philosopher < 5; philosopher++) {
        pthread_join(philosophers[philosopher], NULL);
    }
    for (int fork = 0; fork < 5; fork++) {
        pthread_mutex_destroy(&forks[fork]);
    }
}