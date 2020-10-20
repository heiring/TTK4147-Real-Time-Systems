#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include "io.h"
#include "timespec.h"


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
    long thread_ID = (long) input;
    
    unsigned long duration = 50000000000;
    unsigned long endTime = rt_timer_read() + duration;
	//periodic
	rt_task_set_periodic(NULL,TM_NOW,1000000);
    while(1){
        if(!io_read(thread_ID)){
            io_write(thread_ID,0);
            rt_timer_spin(5 * 1000);
            io_write(thread_ID,1);
        }

        if (rt_timer_read() > endTime) {
	    	rt_printf("Time expired\n");
	    	rt_task_delete(NULL);
		}
		if (rt_task_yield()) {
	    	rt_printf("Task failed to yield\n");
	    	rt_task_delete(NULL);
		}
		rt_task_wait_period(NULL);
    }    
    return NULL;
}

RT_TASK response_task[3];

int main(){
    rt_print_auto_init(1);
    mlockall(MCL_CURRENT | MCL_FUTURE);

    /*pthread_t disturbanceHandle[10];
    for (int i = 0; i < 10; i++) {
        pthread_create(&disturbanceHandle[i], NULL, disturbance, NULL);
    }
	
	for (int i = 0; i < 10; i++) {
        pthread_join(disturbanceHandle[i], NULL);
    }*/

    for (int ID = 0; ID < 3; ID++) {
        rt_task_create(&response_task[ID], NULL, 0, 50, T_CPU(1));
        rt_task_start(&response_task[ID], BRTT_response, (void *) ID + 1);

		//periodic
		//rt_task_set_periodic(&response_task[ID],TM_NOW,1);
    }

    
    
    while(1){
		sleep(30);
	};    

    return 0;
}
