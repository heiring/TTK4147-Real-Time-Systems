#include <stdio.h>
#include "array.h"

void task_A() {
    long xy_size  = 900*1000*1000;     // 8 GB (sizeof(long) = 8 bytes)
    long x_dim    = 10000;
    long y_dim    = xy_size/x_dim;
    long** matrix = malloc(y_dim*sizeof(long*));
    for(long y = 0; y < y_dim; y++){
        matrix[y] = malloc(x_dim*sizeof(long));
        memset(matrix[y], 0, x_dim*sizeof(long));
    }
    printf("Allocation complete (press any key to continue...)\n");
    getchar();
}

void task_BC() {
    Array arr = array_new(20);

    for (long i = 0; i < 30; i++) {
        array_insertBack(&arr, i);
        array_print(arr);
        // printf("%d\n", arr.capacity);
    }

}

void task_D() {
    Array arr = array_new(700 * 1000 * 1000);

    for (long i = 0; i < arr.capacity + 1; i++) {
        array_insertBack(&arr, i);
    }

    for (long i = 0; i < arr.capacity; i++) {
        printf("ptr %ld: %p\n", i, &arr.data[i]);
    }
}

int main(){
    task_D();

    return 0;
}