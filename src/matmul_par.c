/***************************************************************************
 *
 * Sequential version of Matrix-Matrix multiplication
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 1024
#define THREAD_COUNT 8
#define BLOCK_SIZE 512

#define min(x, y) (((x) < (y)) ? (x) : (y))

static double a[SIZE][SIZE];
static double b[SIZE][SIZE];
static double c[SIZE][SIZE];

pthread_t threads[THREAD_COUNT];
pthread_mutex_t threadlock;
int thread_c = 0;

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
    int i;
    int ti;
};

static void matmul_par_block_calc(void* data){
    struct matmul_par_block_calc_args* args = (struct matmul_par_block_calc_args*) data;
    int ti = args->ti;
    int kk, jj;
    int i = args->i;
    int k = args->k;
    int j = args->j;
    free(data);
    for (jj = j; jj < min(j + BLOCK_SIZE, SIZE); ++jj){
        for (kk = k; kk < min(k + BLOCK_SIZE, SIZE); ++kk){
            c[i][jj] += a[i][kk] * b[kk][jj];
        }
        //printf("%d: %f\n", ti, c[i][jj]);
    }
    thread_c--;
    threads[ti] = 0;
    pthread_mutex_unlock(&threadlock);
}
static void
matmul_par_block()
{
    int i, j, k;

    for (k = 0; k < SIZE; k += BLOCK_SIZE){
        for (j = 0; j < SIZE; j += BLOCK_SIZE){
            for (i = 0; i < SIZE; ++i){
                pthread_mutex_lock(&threadlock);
                pthread_mutex_unlock(&threadlock);
                int ti;
                for (ti = 0; threads[ti] != 0; ti++){
                    if (ti > THREAD_COUNT){
                        printf("Fuck\n");
                        exit(0);
                    }
                }
                
                printf("ti: %d\t %d->%d,%d->%d\n", ti, k, min(k+BLOCK_SIZE, SIZE), j, min(j+BLOCK_SIZE, SIZE));
                struct matmul_par_block_calc_args* args = malloc(sizeof(struct matmul_par_block_calc_args));
                args->k = k;
                args->j = j;
                args->i = i;
                args->ti = ti;
                thread_c++;
                pthread_create(&threads[ti], NULL, (void*) matmul_par_block_calc, (void*) args);
                pthread_detach(threads[ti]);

                if (thread_c >= THREAD_COUNT)
                    pthread_mutex_lock(&threadlock);
            }
        }
    }
    for (int ti = 0; ti < THREAD_COUNT; ti++)
        pthread_join(threads[ti], NULL);
}

// https://github.com/EvanPurkhiser/CS-Matrix-Multiplication/blob/master/report.md
// heh..
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
matmul_par_core()
{
     int i, j, k;
 
     pthread_t child_threads[THREAD_COUNT];
     for (i = 0; i < THREAD_COUNT; i++)
         pthread_create(&child_threads[i], NULL, (void*)matmul_calc_row_core,     (void*)(i*128));
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
    init_matrix();
    //matmul_par_core();
    //matmul_seq();
    //matmul_seq_block();
    matmul_par_block();
    //print_matrix();
}

