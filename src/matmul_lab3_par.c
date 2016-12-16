#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 1024
#define THREAD_COUNT 8

static double a[SIZE][SIZE];
static double b[SIZE][SIZE];
static double c[SIZE][SIZE];

static void
init_matrix_seq(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++){
        for (j = 0; j < SIZE; j++) {
            a[i][j] = drand48();
            b[i][j] = drand48();
        }
    }
}

static void
init_matrix_par(void* param)
{
    int i, j;
    i = (int) param;

    for (j = 0; j < SIZE; j++) {
        a[i][j] = drand48();
        b[i][j] = drand48();
    }
}

static void
matmul_seq()
{
    int i, j, k;

    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            c[i][j] = 0.0;
            for (k = 0; k < SIZE; k++)
                c[i][j] = c[i][j] + a[i][k] * b[k][j];
        }
    }
}


static void
matmul_calc_row(void* param)
{
    int i = (int) param;
    int j, k;
    for (j = 0; j < SIZE; j++) {
        c[i][j] = 0.0;
        for (k = 0; k < SIZE; k++)
            c[i][j] = c[i][j] + a[i][k] * b[k][j];
    }
}

static void
matmul_calc_row_core(void* param)
{
    int row = (int) param;
    int i, j, k;
    for (i=row; i<row+128; i++){
        for (j = 0; j < SIZE; j++) {
            c[i][j] = 0.0;
            for (k = 0; k < SIZE; k++)
                c[i][j] = c[i][j] + a[i][k] * b[k][j];
        }
    }
}

static void
matmul_par()
{
    int i, j, k;

    pthread_t child_threads[SIZE];
    for (i = 0; i < SIZE; i++)
        pthread_create(&child_threads[i], NULL, (void*)matmul_calc_row, (void*)i);
    for (i = 0; i < SIZE; i++)
        pthread_join(child_threads[i], NULL);
}

static void
matmul_par_core()
{
    int i, j, k;

    pthread_t child_threads[THREAD_COUNT];
    for (i = 0; i < THREAD_COUNT; i++)
        pthread_create(&child_threads[i], NULL, (void*)matmul_calc_row_core, (void*)(i*128));
    for (i = 0; i < THREAD_COUNT; i++)
        pthread_join(child_threads[i], NULL);
}

static void
print_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++)
            printf(" %7.2f", c[i][j]);
        printf("\n");
    }
}


int
main(int argc, char **argv)
{
    /*    
    int i;

    pthread_t init_threads[SIZE]; 
    for (i = 0; i < SIZE; i++)
        pthread_create(&init_threads[i], NULL, (void*)init_matrix_par, (void*)i);
    for (i = 0; i < SIZE; i++)
        pthread_join(init_threads[i], NULL);
    */ 
    
    init_matrix_seq();


    //matmul_seq();
    //matmul_par();
    matmul_par_core();
    
    //print_matrix();
}

