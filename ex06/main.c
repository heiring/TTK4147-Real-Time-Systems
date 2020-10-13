#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sched.h>
#include "io.h"
#include "timespec.h"

int set_cpu(int cpu_number){
    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_number, &cpu);

    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void* disturbance(void* args) {
    set_cpu(1);   
    while(1){
        asm volatile("" ::: "memory");
    }
}

void* fA(void* args){
    set_cpu(1);
    struct timespec waketime;
    clock_gettime(CLOCK_REALTIME, &waketime);
    struct timespec period = {.tv_sec = 0, .tv_nsec = 1*1000*1000};   
    while(1){
        if(!io_read(1)){
            io_write(1,0);
            usleep(5);
            io_write(1,1);
        }
        waketime = timespec_add(waketime, period);
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
    }    
    return NULL;
}
void* fB(void* args){
    set_cpu(1);
    struct timespec waketime;
    clock_gettime(CLOCK_REALTIME, &waketime);
    struct timespec period = {.tv_sec = 0, .tv_nsec = 1*1000*1000};   
    while(1){
        if(!io_read(2)){
            io_write(2,0);
            usleep(5);
            io_write(2,1);
        }
        waketime = timespec_add(waketime, period);
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
    }    
    return NULL;
}
void* fC(void* args){
    set_cpu(1);
    struct timespec waketime;
    clock_gettime(CLOCK_REALTIME, &waketime);
    struct timespec period = {.tv_sec = 0, .tv_nsec = 1*1000*1000};   
    while(1){
        if(!io_read(3)){
            io_write(3,0);
            usleep(5);
            io_write(3,1);
        }
        waketime = timespec_add(waketime, period);
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
    }    
    return NULL;
}

void print_periodic() {
    struct timespec waketime;
    clock_gettime(CLOCK_REALTIME, &waketime);
    struct timespec period = {.tv_sec = 0, .tv_nsec = 500*1000*1000};
    while(1){
        printf("Hello\n");
        fflush(stdout);
        
        // sleep
        waketime = timespec_add(waketime, period);
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
    }
}



int main(){
    io_init();

    pthread_t disturbanceHandle[10];
    for (int i = 0; i < 10; i++) {
        pthread_create(&disturbanceHandle[i], NULL, disturbance, NULL);
    }

    pthread_t threadHandle_1;
    pthread_t threadHandle_2;
    pthread_t threadHandle_3;
    pthread_create(&threadHandle_1, NULL, fA, NULL);
    pthread_create(&threadHandle_2, NULL, fB, NULL);
    pthread_create(&threadHandle_3, NULL, fC, NULL);
    pthread_join(threadHandle_1, NULL);
    pthread_join(threadHandle_2, NULL);
    pthread_join(threadHandle_3, NULL);

    for (int i = 0; i < 10; i++) {
        pthread_join(disturbanceHandle[i], NULL);
    }

    // print_periodic();

    return 0;
}