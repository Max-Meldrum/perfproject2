/***************************************************************************
 *
 * Sequential version of Matrix-Matrix multiplication
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef THREAD_COUNT
    #define THREAD_COUNT 8
#endif

#define SIZE 1024
#define BLOCK_SIZE 512
#define BLOCK_SIZE_W 512
#define BLOCK_SIZE_H 256

#define min(x, y) (((x) < (y)) ? (x) : (y))

static double a[SIZE][SIZE];
static double b[SIZE][SIZE];
static double c[SIZE][SIZE];

pthread_mutex_t locks[SIZE][SIZE];
pthread_t threads[THREAD_COUNT];

static void
init_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++) {
	    /* Simple initialization, which enables us to easy check
	     * the correct answer. Each element in c will have the same 
	     * value as SIZE after the matmul operation.
	     */
	    a[i][j] = 1.0;
	    b[i][j] = 1.0;
        //temp
        c[i][j] = 0.0;
        }
}


struct matmul_par_block_calc_args {
    int k;
    int j;
    int ti;
};

static void matmul_par_block_calc(void* data){
    struct matmul_par_block_calc_args* args = (struct matmul_par_block_calc_args*) data;
    int ti = args->ti;
    int kk, jj;
    int i;
    int k = args->k;
    int j = args->j;
    free(data);
    for (i = 0; i < SIZE; ++i){
        for (jj = j; jj < min(j + BLOCK_SIZE_H, SIZE); ++jj){
            for (kk = k; kk < min(k + BLOCK_SIZE_W, SIZE); ++kk){
                pthread_mutex_lock(&locks[i][jj]);
                c[i][jj] += a[i][kk] * b[kk][jj];
                pthread_mutex_unlock(&locks[i][jj]);
            }
        }
    }
}
static void
matmul_par_block()
{
    int i, j, k;

    int ti = 0;
    for (k = 0; k < SIZE; k += BLOCK_SIZE_W){
        for (j = 0; j < SIZE; j += BLOCK_SIZE_H){
            struct matmul_par_block_calc_args* args = malloc(sizeof(struct matmul_par_block_calc_args));
            args->k = k;
            args->j = j;
            args->ti = ti;
            pthread_create(&threads[ti], NULL, (void*) matmul_par_block_calc, (void*) args);
            ti++;
        }
    }
    for (int ti = 0; ti < THREAD_COUNT; ti++)
        pthread_join(threads[ti], NULL);
}

static void
matmul_seq_block()
{
    int i, j, k, kk, jj;

    for (k = 0; k < SIZE; k += BLOCK_SIZE) 
        for (j = 0; j < SIZE; j += BLOCK_SIZE) 
            for (i = 0; i < SIZE; ++i) 
                for (jj = j; jj < min(j + BLOCK_SIZE, SIZE); ++jj) 
                    for (kk = k; kk < min(k + BLOCK_SIZE, SIZE); ++kk) 
                        c[i][jj] += a[i][kk] * b[kk][jj];
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
    init_matrix();
    //matmul_par_core();
    //matmul_seq();
    if (THREAD_COUNT <= 1)
        matmul_seq_block();
    else
        matmul_par_block();
    //print_matrix();
}

