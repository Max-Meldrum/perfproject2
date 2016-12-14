/***************************************************************************
 *
 * Sequential version of Quick sort
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#define KILO (1024)
#define MEGA (1024*1024)
#define MAX_ITEMS (64*MEGA)

#define swap(v, a, b) {unsigned tmp; tmp=v[a]; v[a]=v[b]; v[b]=tmp;}

#ifndef THREAD_COUNT
    #define THREAD_COUNT 8
#endif

int thread_count = 1;
pthread_t threads[THREAD_COUNT];

static int *v;

static void
print_array(void)
{
    int i;

    for (i = 0; i < MAX_ITEMS; i++)
        printf("%d ", v[i]);
    printf("\n");
}

static void
init_array(void)
{
    int i;

    v = (int *) malloc(MAX_ITEMS*sizeof(int));
    for (i = 0; i < MAX_ITEMS; i++)
        v[i] = rand();
}

static unsigned
partition(int *v, unsigned low, unsigned high, unsigned pivot_index)
{
    /* move pivot to the bottom of the vector */
    if (pivot_index != low)
        swap(v, low, pivot_index);

    pivot_index = low;
    low++;

    /* invariant:
     * v[i] for i less than low are less than or equal to pivot
     * v[i] for i greater than high are greater than pivot
     */

    /* move elements into place */
    while (low <= high) {
        if (v[low] <= v[pivot_index])
            low++;
        else if (v[high] > v[pivot_index])
            high--;
        else
            swap(v, low, high);
    }

    /* put pivot back between two groups */
    if (high != pivot_index)
        swap(v, pivot_index, high);
    return high;
}

struct qsortparams {
    int *v;
    unsigned low;
    unsigned high;
    unsigned depth;
};

static void
quick_sort(void* data)
{
    struct qsortparams* pars = (struct qsortparams*) data;
    int* v = pars->v;
    unsigned low = pars->low;
    unsigned high = pars->high;
    unsigned depth = pars->depth;
    free(data);

    unsigned pivot_index;

    /* no need to sort a vector of zero or one element */
    if (low >= high)
        return;

    /* select the pivot value */
    pivot_index = (low+high)/2;

    /* partition the vector */
    pivot_index = partition(v, low, high, pivot_index);

    /* sort the two sub arrays */
    if (low < pivot_index){
        struct qsortparams* params = malloc(sizeof(struct qsortparams));
        params->v = v;
        params->low = low;
        params->high = pivot_index-1;
        params->depth = depth+1;
        if ( depth < 10 && (THREAD_COUNT >> depth) >= 1 && thread_count < THREAD_COUNT){
            //printf("d=%d at %d,%d and %d\n", depth, low, high, THREAD_COUNT >> depth);
            thread_count++;
            pthread_create(&threads[thread_count-1], NULL, (void*) quick_sort, (void*) params);
        }
        else
            quick_sort((void*)params);
    }
    if (pivot_index < high){
        struct qsortparams* params = malloc(sizeof(struct qsortparams));
        params->v = v;
        params->low = pivot_index+1;
        params->high = high;
        params->depth = depth+1;
        quick_sort((void*)params);
    }
}

int
main(int argc, char **argv)
{
    init_array();
    //print_array();
    struct qsortparams* params = malloc(sizeof(struct qsortparams));
    params->v = v;
    params->low = 0;
    params->high = MAX_ITEMS-1;
    params->depth = 1;
    quick_sort((void*) params);
    for (int ti=0; ti < THREAD_COUNT; ti++){
        pthread_join(threads[ti], NULL);
    }
    print_array();
}

