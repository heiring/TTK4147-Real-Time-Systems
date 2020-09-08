#include "timespec.h"
#include <sys/times.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <x86intrin.h>
#include <sched.h>

void busy_wait_clock_gettime(){
    struct timespec now;
    struct timespec t = {1,0};
    struct timespec then = timespec_add(now, t);
    clock_gettime(CLOCK_MONOTONIC, &now);
    while(timespec_cmp(now, then) < 0){
        for(int i = 0; i < 10000; i++){}
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
}

void busy_wait_times(){
    

    static struct tms start;
    clock_t st_time = times(&start);

    static struct tms end;
    clock_t end_time = times(&end);

    long one_second = sysconf(_SC_CLK_TCK);

    while(end_time - st_time< one_second){

        end_time = times(&end);
    }

}

void latency_rdtsc() {
    for (int i = 0; i < 10*1000*1000; i++) {
        __rdtsc();
    }
}

void latency_clock_gettime() {
    struct timespec now;
    for (int i = 0; i < 10*1000*1000; i++) {
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
}

void latency_times() {
    static struct tms start;
    for (int i = 0; i < 10*1000*1000; i++) {
        times(&start);
    }    
}

void resolution_rdtsc() {
    int ns_max = 50;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int)*ns_max);
    
    for (int i = 0; i < 10*1000*1000; i++) {
        unsigned long t1 = __rdtsc();
        unsigned long t2 = __rdtsc();
        int ns = (t2 - t1) / 2.67;

        if (ns >= 0 && ns < ns_max) {
            histogram[ns]++;
        }
    }
    for(int i = 0; i < ns_max; i++){
        printf("%d\n", histogram[i]);
    }
}

void resolution_clock_gettime() {
    int ns_max = 50;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int)*ns_max);
    struct timespec t1;
    struct timespec t2;    
    
    for (int i = 0; i < 10*1000*1000; i++) {
        clock_gettime(CLOCK_MONOTONIC, &t1);        
        clock_gettime(CLOCK_MONOTONIC, &t2);
        
        int ns = (t2.tv_nsec - t1.tv_nsec);

        if (ns >= 0 && ns < ns_max) {
            histogram[ns]++;
        }
    }
    for(int i = 0; i < ns_max; i++){
        printf("%d\n", histogram[i]);
    }
}

void resolution_times() {
    int ns_max = 50;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int)*ns_max);
    static struct tms start;
    static struct tms end;
    long ticks_per_sec = sysconf(_SC_CLK_TCK);
    
    for (int i = 0; i < 10*1000*1000; i++) {
        clock_t st_time = times(&start);
        clock_t end_time = times(&end);

        int ns = (end_time - st_time) * 1000 * 1000 * 1000/ ticks_per_sec;

        if (ns >= 0 && ns < ns_max) {
            histogram[ns]++;
        }
    }
    for(int i = 0; i < ns_max; i++){
        printf("%d\n", histogram[i]);
    }
}

void taskC() {
    int ns_max = 500;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int)*ns_max);
    struct timespec t1;
    struct timespec t2;    
    //int nsc =0;
    for (int i = 0; i < 10*1000*1000; i++) {
        
        //unsigned long c1 = __rdtsc();
        clock_gettime(CLOCK_MONOTONIC, &t1);
        sched_yield();
        clock_gettime(CLOCK_MONOTONIC, &t2);
        //unsigned long c2 = __rdtsc();

        //int nsc = (c2 - c1) / 2.67;
        
        int ns = (t2.tv_nsec - t1.tv_nsec);

        if (ns >= 0 && ns < ns_max) {
            histogram[ns]++;
        }
    }
    for(int i = 0; i < ns_max; i++){
        printf("%d\n", histogram[i]);
    }
    //printf("Kernel switch time: %d\n",nsc);
}

int main(){
    /* TASK A */
    // sleep(1);
    // busy_wait_times();
    // busy_wait_clock_gettime();

    /* TASK B */
    // latency_rdtsc();
    // latency_clock_gettime();
    // latency_times();
    // resolution_rdtsc();
    // resolution_clock_gettime();
    resolution_times();

    /* TASK C */
    //taskC();


    return 0;
}

