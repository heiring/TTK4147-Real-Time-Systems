#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include <native/sem.h>
#include <native/mutex.h>

#include "taskAC.h"

RT_TASK tasksDE[2];
RT_SEM sync_sem;
RT_MUTEX mxA;
RT_MUTEX mxB;

void icpp_lock(RT_MUTEX* mx, RT_TASK task);

void icpp_unlock(RT_MUTEX* mx, RT_TASK task, int base_prio);

void low_taskD(void* input){
	//Barrier
	rt_sem_p(&sync_sem, TM_INFINITE);

	//Lock A
	rt_printf("L A requested\n");	
	rt_mutex_acquire(&mxA, TM_INFINITE);	
	rt_printf("L A acquired\n");

	//Busy wait 3 time units
	for (int i = 0; i < 3; i++) {
		rt_printf("L busy wait\n");
		rt_timer_spin(100*1000*1000);
	}
	
	//Lock B
	rt_printf("L B requested\n");
	rt_mutex_acquire(&mxB, TM_INFINITE);
	rt_printf("L B acquired\n");

	//Busy wait 3 time units
	for (int i = 0; i < 3; i++) {
		rt_printf("L busy wait\n");
		rt_timer_spin(100*1000*1000);
	}

	//Unlock B
	rt_printf("L B released\n");
	rt_mutex_release(&mxB);

	//Unlock A
	rt_printf("L A released\n");
	rt_mutex_release(&mxA);

	//Busy wait 1 time units
	for (int i = 0; i < 1; i++) {
		rt_printf("L busy wait\n");
		rt_timer_spin(100*1000*1000);
	}	
}

void hi_taskD(void* input){
	//Barrier
	rt_sem_p(&sync_sem, TM_INFINITE);
	
	//Sleep 1 time unit
	rt_task_sleep(1 * 100 * 1000 * 1000);

	//Lock B
	rt_printf("H B requested\n");
	rt_mutex_acquire(&mxB, TM_INFINITE);
	rt_printf("H B acquired\n");

	//Busy wait 1 time units
	for (int i = 0; i < 1; i++) {
		rt_printf("H busy wait\n");
		rt_timer_spin(100*1000*1000);
	}

	//Lock A
	rt_printf("H A requested\n");	
	rt_mutex_acquire(&mxA, TM_INFINITE);	
	rt_printf("H A acquired\n");

	//Busy wait 2 time units
	for (int i = 0; i < 2; i++) {
		rt_printf("H busy wait\n");
		rt_timer_spin(100*1000*1000);
	}

	//Unlock A
	rt_printf("H A released\n");
	rt_mutex_release(&mxA);

	//Unlock B
	rt_printf("H B released\n");
	rt_mutex_release(&mxB);

	//Busy wait 1 time units
	for (int i = 0; i < 1; i++) {
		rt_printf("H busy wait\n");
		rt_timer_spin(100*1000*1000);
	}
}

void problemD(){
	// Init semaphore
	rt_sem_create(&sync_sem, NULL, 0, S_PRIO);
	rt_mutex_create(&mxA, NULL);
	rt_mutex_create(&mxB, NULL);
	
	//create and start tasks
	rt_task_create(&tasksDE[0], NULL, 0, 50, T_CPU(1));
    rt_task_start(&tasksDE[0], low_taskD, NULL);
	rt_task_create(&tasksDE[1], NULL, 0, 51, T_CPU(1));
    rt_task_start(&tasksDE[1], hi_taskD, NULL);

	//synchronize tasks with barrier
	rt_task_sleep(100 * 1000 * 1000);
	rt_sem_broadcast(&sync_sem);
	
	rt_task_sleep(5 * 1000 * 1000 * 1000);
	printf("Tasks deadlocked\n");

	rt_printf("Deleting semaphores\n");
	//delete semaphores
	rt_sem_delete(&sync_sem);
	rt_mutex_delete(&mxA);	
	rt_mutex_delete(&mxB);
}

void low_taskE(void* input){
	//Barrier
	rt_sem_p(&sync_sem, TM_INFINITE);

	//Lock A
	rt_printf("L A requested\n");	
	icpp_lock(&mxA, tasksDE[0]);	
	rt_printf("L A acquired\n");

	//Busy wait 3 time units
	for (int i = 0; i < 3; i++) {
		rt_printf("L busy wait\n");
		rt_timer_spin(100*1000*1000);
	}
	
	//Lock B
	rt_printf("L B requested\n");
	icpp_lock(&mxB, tasksDE[0]);
	rt_printf("L B acquired\n");

	//Busy wait 3 time units
	for (int i = 0; i < 3; i++) {
		rt_printf("L busy wait\n");
		rt_timer_spin(100*1000*1000);
	}

	//Unlock B
	rt_printf("L B released\n");
	icpp_unlock(&mxB, tasksDE[0], 60);

	//Unlock A
	rt_printf("L A released\n");
	icpp_unlock(&mxA, tasksDE[0], 50);

	//Busy wait 1 time units
	for (int i = 0; i < 1; i++) {
		rt_printf("L busy wait\n");
		rt_timer_spin(100*1000*1000);
	}	
}

void hi_taskE(void* input){
	//Barrier
	rt_sem_p(&sync_sem, TM_INFINITE);
	
	//Sleep 1 time unit
	rt_task_sleep(1 * 100 * 1000 * 1000);

	//Lock B
	rt_printf("H B requested\n");
	icpp_lock(&mxB, tasksDE[1]);
	rt_printf("H B acquired\n");

	//Busy wait 1 time units
	for (int i = 0; i < 1; i++) {
		rt_printf("H busy wait\n");
		rt_timer_spin(100*1000*1000);
	}

	//Lock A
	rt_printf("H A requested\n");	
	icpp_lock(&mxA, tasksDE[1]);	
	rt_printf("H A acquired\n");

	//Busy wait 2 time units
	for (int i = 0; i < 2; i++) {
		rt_printf("H busy wait\n");
		rt_timer_spin(100*1000*1000);
	}

	//Unlock A
	rt_printf("H A released\n");
	icpp_unlock(&mxA, tasksDE[1], 51);

	//Unlock B
	rt_printf("H B released\n");
	icpp_unlock(&mxB, tasksDE[1], 51);

	//Busy wait 1 time units
	for (int i = 0; i < 1; i++) {
		rt_printf("H busy wait\n");
		rt_timer_spin(100*1000*1000);
	}
}

void problemE(){
	// Init semaphore
	rt_sem_create(&sync_sem, NULL, 0, S_PRIO);
	rt_mutex_create(&mxA, NULL);
	rt_mutex_create(&mxB, NULL);
	
	//create and start tasks
	rt_task_create(&tasksDE[0], NULL, 0, 50, T_CPU(1));
    rt_task_start(&tasksDE[0], low_taskE, NULL);
	rt_task_create(&tasksDE[1], NULL, 0, 51, T_CPU(1));
    rt_task_start(&tasksDE[1], hi_taskE, NULL);

	//synchronize tasks with barrier
	rt_task_sleep(100 * 1000 * 1000);
	rt_sem_broadcast(&sync_sem);

	rt_printf("Deleting semaphores\n");
	//delete semaphores
	rt_sem_delete(&sync_sem);
	rt_mutex_delete(&mxA);	
	rt_mutex_delete(&mxB);
}

void icpp_lock(RT_MUTEX* mx, RT_TASK task) {
	rt_mutex_acquire(mx, TM_INFINITE);
	rt_task_set_priority(&task, 60);	
}

void icpp_unlock(RT_MUTEX* mx, RT_TASK task, int base_prio) {
	rt_mutex_release(mx);
	rt_task_set_priority(&task, base_prio);
}

int main(){
    rt_print_auto_init(1);
    mlockall(MCL_CURRENT | MCL_FUTURE);

	// Make main Xenomai task
	rt_task_shadow(NULL, NULL, 1, T_CPU(1));

	problemE();
    
    /*while(1){
		sleep(30);
	};*/    

    return 0;
}














