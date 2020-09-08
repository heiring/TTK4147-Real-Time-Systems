#include "timespec.c"


void busy_wait(struct timespec t);

void main(){
    struct timespec one_sec = {1,0};
    busy_wait(one_sec);
}


void busy_wait(struct timespec t){
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    struct timespec then = timespec_add(now, t);
    while(timespec_cmp(now, then) < 0){
        for(int i = 0; i < 10000; i++){}
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
}