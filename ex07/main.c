#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sched.h>
#include "io.h"
#include "timespec.h"

#include <sys/mman.h>
#include <signal.h>
#include <native/task.h>


int set_cpu(int cpu_number){
    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_number, &cpu);

    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
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

void* disturbance(void* args) {
    set_cpu(1);   
    while(1){
        asm volatile("" ::: "memory");
    }
}

struct args {
    int thread_ID;
};

// function to be executed by task
void BRTT_response(void *input)
{
    int thread_ID = ((struct args*)input)->thread_ID;

    //struct timespec waketime;
    //clock_gettime(CLOCK_REALTIME, &waketime);
    //struct timespec period = {.tv_sec = 0, .tv_nsec = 1*1000*1000};
   
    while(1){
        if(!io_read(thread_ID + 1)){
            io_write(thread_ID + 1,0);
            rt_timer_spin(5 * 1000);
            io_write(thread_ID + 1,1);
        }
        //waketime = timespec_add(waketime, period);
        //clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
    }    
    return NULL;
}

RT_TASK response_task[3];

int main(){
    io_init();
    mlockall(MCL_CURRENT | MCL_FUTURE);

    // pthread_t disturbanceHandle[10];
    // for (int i = 0; i < 10; i++) {
    //     pthread_create(&disturbanceHandle[i], NULL, disturbance, NULL);
    // }
    char  str[20];

    sprintf(str,"BRTT_response_task");
    for (int ID = 0; ID < 3; ID++) {
        struct args *input = (struct args *)malloc(sizeof(struct args));
	input->thread_ID = ID;
        rt_task_create(&response_task[ID], str, 0, 50, T_CPU(1));
        rt_task_start(&response_task[ID], &BRTT_response, (void *)input);
    }

    // for (int i = 0; i < 10; i++) {
    //     pthread_join(disturbanceHandle[i], NULL);
    // }
    
    while(1);    

    return 0;
}
