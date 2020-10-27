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

RT_SEM sync_sem;
RT_SEM resc_sem;
RT_MUTEX mx;

void task(void* input) {
	long data = (long) input;
	
	rt_printf("Decrementing sem, prio %d\n", data);
	rt_sem_p(&sync_sem, TM_INFINITE);
	rt_printf("Synchronized, prio %d\n", data);

	/*unsigned long duration = 50000000000;
    unsigned long endTime = rt_timer_read() + duration;
	while(1) {
		if (rt_timer_read() > endTime) {
	    	rt_printf("Time expired\n");
	    	rt_task_delete(NULL);
		}
		if (rt_task_yield()) {
	    	rt_printf("Task failed to yield\n");
	    	rt_task_delete(NULL);
		}
		rt_task_wait_period(NULL);
	}*/
}

void busybusy(int ms){
	/*unsigned long duration = ms * 1000000;
    unsigned long endTime = rt_timer_read() + duration;
	while(rt_timer_read() > endTime);*/
	rt_timer_spin(ms*1000*1000);
}

void low_taskB(void* input){
	rt_sem_p(&sync_sem, TM_INFINITE);

	rt_sem_p(&resc_sem, TM_INFINITE);
	rt_printf("resource locked by L\n");
	busybusy(3 * 100);
	rt_printf("L released\n");
	rt_sem_v(&resc_sem);
	rt_printf("L finished\n");
}

void med_taskB(void* input){
	rt_sem_p(&sync_sem, TM_INFINITE);

	rt_task_sleep(1 * 100 * 1000 * 1000);
	busybusy(5 * 100);
	rt_printf("M finished\n");
}

void hi_taskB(void* input){
	rt_sem_p(&sync_sem, TM_INFINITE);

	rt_task_sleep(2 * 100 * 1000 * 1000);
	rt_sem_p(&resc_sem, TM_INFINITE);
	busybusy(2 * 100);
	rt_sem_v(&resc_sem);
	rt_printf("H finished\n");
}

void low_taskC(void* input){
	struct rt_task_info temp;

	//Barrier
	rt_sem_p(&sync_sem, TM_INFINITE);

	rt_task_inquire(NULL, &temp);
	rt_printf("L: Base prio: %i, Current prio: %i\n", temp.bprio, temp.cprio);
	
	rt_printf("L mx requested\n");
	rt_mutex_acquire(&mx, TM_INFINITE);
	rt_printf("L mx acquired\n");

	for (int i = 0; i < 3; i++) {
		rt_printf("L busy wait\n");
		rt_timer_spin(100*1000*1000);
	}

	rt_task_inquire(NULL, &temp);
	rt_printf("L: Base prio: %i, Current prio: %i\n", temp.bprio, temp.cprio);
	rt_printf("L mx released\n");
	rt_mutex_release(&mx);
	rt_printf("L finished\n");
}

void med_taskC(void* input){
	//Barrier
	rt_sem_p(&sync_sem, TM_INFINITE);

	rt_task_sleep(1 * 100 * 1000 * 1000);
	for (int i = 0; i < 5; i++) {
		rt_printf("M busy wait\n");
		rt_timer_spin(100*1000*1000);
	}
	rt_printf("M finished\n");
}

void hi_taskC(void* input){
	//Barrier
	rt_sem_p(&sync_sem, TM_INFINITE);

	rt_task_sleep(2 * 100 * 1000 * 1000);
	rt_printf("H mx requested\n");
	rt_mutex_acquire(&mx, TM_INFINITE);
	rt_printf("H mx acquired\n");
	for (int i = 0; i < 2; i++) {
		rt_printf("H busy wait\n");
		rt_timer_spin(100*1000*1000);
	}
	rt_printf("H mx released\n");
	rt_mutex_release(&mx);
	rt_printf("H finished\n");
}

RT_TASK tasks[3];


void problemA(){
	// Init semaphore
	rt_sem_create(&sync_sem, NULL, 0, S_PRIO);

    // Create and start tasks
    rt_task_create(&tasks[0], NULL, 0, 50, T_CPU(1));
    rt_task_start(&tasks[0], task, 50);
	rt_task_create(&tasks[1], NULL, 0, 51, T_CPU(1));
    rt_task_start(&tasks[1], task, 51);
	
	rt_task_sleep(100 * 1000 * 1000);

	rt_sem_broadcast(&sync_sem);
	rt_printf("Sem broadcasted\n");

	rt_task_sleep(100 * 1000 * 1000);
	
	rt_sem_delete(&sync_sem);
}

void problemB(){
	// Init semaphore
	rt_sem_create(&sync_sem, NULL, 0, S_PRIO);
	rt_sem_create(&resc_sem, NULL, 1, S_PRIO);
	
	//create and start tasks
	rt_task_create(&tasks[0], NULL, 0, 50, T_CPU(1));
    rt_task_start(&tasks[0], low_taskB, NULL);
	rt_task_create(&tasks[1], NULL, 0, 51, T_CPU(1));
    rt_task_start(&tasks[1], med_taskB, NULL);
	rt_task_create(&tasks[2], NULL, 0, 53, T_CPU(1));
    rt_task_start(&tasks[2], hi_taskB, NULL);	

	//synchronize tasks with barrier
	rt_task_sleep(100 * 1000 * 1000);
	rt_sem_broadcast(&sync_sem);
	rt_printf("tasks released at barrier\n");

	//delete semaphores
	rt_sem_delete(&sync_sem);
	rt_sem_delete(&resc_sem);
}

void problemC(){
	// Init semaphore
	rt_sem_create(&sync_sem, NULL, 0, S_PRIO);
	rt_mutex_create(&mx, NULL);
	
	//create and start tasks
	rt_task_create(&tasks[0], NULL, 0, 50, T_CPU(1));
    rt_task_start(&tasks[0], low_taskC, NULL);
	rt_task_create(&tasks[1], NULL, 0, 51, T_CPU(1));
    rt_task_start(&tasks[1], med_taskC, NULL);
	rt_task_create(&tasks[2], NULL, 0, 53, T_CPU(1));
    rt_task_start(&tasks[2], hi_taskC, NULL);	

	//synchronize tasks with barrier
	rt_task_sleep(100 * 1000 * 1000);
	rt_sem_broadcast(&sync_sem);
	rt_printf("tasks released at barrier\n");

	//delete semaphores
	rt_sem_delete(&sync_sem);
	rt_mutex_delete(&mx);
}












