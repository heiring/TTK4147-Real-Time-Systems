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

RT_TASK hello_task;

// function to be executed by task
void helloWorld(void *arg)
{
  RT_TASK_INFO curtaskinfo;

  printf("Hello World!\n");

  // inquire current task
  rt_task_inquire(NULL,&curtaskinfo);

  // print task name
  printf("Task name : %s \n", curtaskinfo.name);
}

int main(){
    io_init();
    mlockall(MCL_CURRENT | MCL_FUTURE);

    // pthread_t disturbanceHandle[10];
    // for (int i = 0; i < 10; i++) {
    //     pthread_create(&disturbanceHandle[i], NULL, disturbance, NULL);
    // }
    char  str[10];

    printf("start task\n");
    sprintf(str,"hello");

    /* Create task
    * Arguments: &task,
    *            name,
    *            stack size (0=default),
    *            priority,
    *            mode (FPU, start suspended, ...)
    */
    rt_task_create(&hello_task, str, 0, 50, 0);

    /*  Start task
    * Arguments: &task,
    *            task function,
    *            function argument
    */
    rt_task_start(&hello_task, &helloWorld, 0);

    // for (int i = 0; i < 10; i++) {
    //     pthread_join(disturbanceHandle[i], NULL);
    // }

    return 0;
}