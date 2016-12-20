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

// Macro needed for the blocked 2d matrix multiplication algorithm
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
        // Set c[i][j] to 0.0 in the initialization instead of in the algorithm
        c[i][j] = 0.0;
        }
}

// Struct that is used to send in arguments to the threaded block calc function
struct matmul_par_block_calc_args {
    int k;
    int j;
};

static void matmul_par_block_calc(void* data){
    struct matmul_par_block_calc_args* args = (struct matmul_par_block_calc_args*) data;
    int kk, jj;
    int i;
    int k = args->k;
    int j = args->j;
    free(data); // deallocate memory of the struct
    for (i = 0; i < SIZE; ++i){
        // Istead of iterating over BLOCK_SIZE like in the sequential implementation, iterate over BLOCK_SIZE_W and BLOCK_SIZE_H to get 8 blocks for our 8 threads
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
    int j, k;

    int ti = 0;
    // Istead of iterating over BLOCK_SIZE like in the sequential implementation, iterate over BLOCK_SIZE_W and BLOCK_SIZE_H to get 8 blocks for our 8 threads
    for (k = 0; k < SIZE; k += BLOCK_SIZE_W){
        for (j = 0; j < SIZE; j += BLOCK_SIZE_H){
            struct matmul_par_block_calc_args* args = malloc(sizeof(struct matmul_par_block_calc_args));
            args->k = k;
            args->j = j;
            pthread_create(&threads[ti], NULL, (void*) matmul_par_block_calc, (void*) args);
            ti++;
        }
    }
    // Wait for all the threads to finish by the join command
    for (int ti = 0; ti < THREAD_COUNT; ti++)
        pthread_join(threads[ti], NULL);
}

// https://github.com/EvanPurkhiser/CS-Matrix-Multiplication/blob/master/report.md
// 2d blocked matrix multiplication
// More info in the report
static void
matmul_seq_block()
{
    int i, j, k, kk, jj;

    for (k = 0; k < SIZE; k += BLOCK_SIZE) // Partition columns into blocks
        for (j = 0; j < SIZE; j += BLOCK_SIZE) // Partition rows into block
            for (i = 0; i < SIZE; ++i) // Do one iteration over each cell
                for (jj = j; jj < min(j + BLOCK_SIZE, SIZE); ++jj) // Iterate over each column in current block
                    for (kk = k; kk < min(k + BLOCK_SIZE, SIZE); ++kk) // Iterate ver each row in current block
                        // c[i][jj] will increase the value at i,jj
                        // (every row, all columns in block)
                        // with a[i][kk] * b[kk][jj]
                        // (each row in a, every columns in block)
                        // (every row in b in block, every column in b in block)
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

