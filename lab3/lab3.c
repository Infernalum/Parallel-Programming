#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define _THREADS 5
#define _SIZE 1e7
#define _ARRAYS 1
#define _FREQUENCY 0.1

int comp(const void *elem1, const void *elem2)
{
    int f = *((int *)elem1);
    int s = *((int *)elem2);
    if (f > s)
        return 1;
    if (f < s)
        return -1;
    return 0;
}

int isEquals(int *array1, int *array2)
{
    for (int i = 0; i < _SIZE; ++i)
        if (array1[i] != array2[i])
            return 0;
    return 1;
}

int isSort(int *array)
{
    for (int i = 0; i < _SIZE - 1; ++i)
        if (array[i] > array[i + 1])
            return 0;
    return 1;
}

int *InsertionSort(int *array)
{
    int *object = (int *)malloc(sizeof(int) * _SIZE);
    for (int i = 0; i < _SIZE; ++i)
        object[i] = array[i];
    double start, end;

    /* Sorting */
    start = omp_get_wtime();
    for (int i = 1; i < _SIZE; ++i)
    {
        int key = object[i];
        int j = i - 1;
        while (j >= 0 && object[j] > key)
        {
            object[j + 1] = object[j];
            --j;
        }
        object[j + 1] = key;
    }
    end = omp_get_wtime();
    /* Show sorted array */
    //for (int i = 0; i < _SIZE; ++i)
    //    printf("%d, ", object[i]);
    //printf("\n");
    printf("Insertion Sort time: \t\t%.8f;\n", end - start);
    printf("Is it sorted? - \t%d;\n", isSort(object));
    return object;
}

int *ShellSort(int *array)
{
    int *object = (int *)malloc(sizeof(int) * _SIZE);
    for (int i = 0; i < _SIZE; ++i)
        object[i] = array[i];
    double start, end;
    start = omp_get_wtime();

    for (int step = _SIZE / 2; step > 0; step /= 2)
    {
        for (int i = 0; i < step; ++i)
        {
            for (int j = i + step; j < _SIZE; j += step)
            {
                int key = object[j], p = j - step;
                {
                    while (p >= 0 && object[p] > key)
                    {
                        object[p + step] = object[p];
                        p -= step;
                    }
                    object[p + step] = key;
                }
            }
        }

        //for (int i = 0; i < _SIZE; ++i)
        //    printf("%d, ", object[i]);
        //printf("\n");
    }
    end = omp_get_wtime();
    /* Show sorted array */
    //for (int i = 0; i < size; ++i)
    //    printf("%d, ", object[i]);
    //printf("\n");
    printf("Shell Sort time: \t\t%.8f;\n", end - start);
    printf("Is it sorted? - \t%d;\n", isSort(object));
    return object;
}

int *ShellParallelSort(int *array)
{
    int *object = (int *)malloc(sizeof(int) * _SIZE);
    for (int i = 0; i < _SIZE; ++i)
        object[i] = array[i];
    double start, end;
    start = omp_get_wtime();

omp_set_num_threads(_THREADS);
#pragma omp parallel shared(object)
    {
        for (int step = _SIZE / 2; step > 0; step /= 2)
        {
#pragma omp for
            for (int i = 0; i < step; ++i)
            {
                //printf("step: %d; thread: %d; i: %d;\n", step, omp_get_thread_num(), i);
                for (int j = i + step; j < _SIZE; j += step)
                {
                    int key = object[j], p = j - step;
                    {
                        while (p >= 0 && object[p] > key)
                        {
                            object[p + step] = object[p];
                            p -= step;
                        }
                        object[p + step] = key;
                    }
                }
            }
            // #pragma omp single
            //             {
            //                 for (int i = 0; i < _SIZE; ++i)
            //                     printf("%d, ", object[i]);
            //                 printf("\n");
            //             }
        }
    }

    end = omp_get_wtime();
    /* Show sorted array */
    //for (int i = 0; i < _SIZE; ++i)
    //    printf("%d, ", object[i]);
    //printf("\n");
    printf("Shell Parallel Sort time: \t%.8f;\n", end - start);
    printf("Is it sorted? - \t%d;\n", isSort(object));
    return object;
}

int main(int argc, char **argv)
{
    int seed = 915482;
    double start = 0, end = 0, _time = 0;
    double consecutive_time = 0, parallel_time = 0;
    double average_time = 0;

    int *array = 0;
    int *pattern = 0;
    int *cmp1 = 0;
    int *cmp2 = 0;
    int *cmp3 = 0;
    for (int i = 0; i < _ARRAYS; ++i)
    {
        printf("seed: %d;\n", seed);
        srand(seed);
        array = (int *)malloc(_SIZE * sizeof(int));
        for (int i = 0; i < _SIZE; i++)
            array[i] = rand() % (int)(_SIZE / _FREQUENCY + 1 - 0) + 0;
        pattern = (int *)malloc(_SIZE * sizeof(int));
        for (int i = 0; i < _SIZE; ++i)
            pattern[i] = array[i];

        double start, end;
        start = omp_get_wtime();
        qsort(pattern, _SIZE, sizeof(*pattern), comp);
        end = omp_get_wtime();
        //for (int i = 0; i < _SIZE; ++i)
        //    printf("%d, ", array[i]);
        //printf("\n");
        printf("QuickSort time: \t\t%.8f;\nIs it sorted? - %d;\n", end - start, isSort(pattern));
        //cmp1 = InsertionSort(array);
        //printf("Are they equal? - \t%d;\n", isEquals(cmp1, pattern));
        cmp2 = ShellSort(array);
        printf("Are they equal? - \t%d;\n", isEquals(cmp2, pattern));
        cmp3 = ShellParallelSort(array);
        printf("Are they equal? - \t%d;\n", isEquals(cmp3, pattern));
        seed += 1000;
        free(array);
        free(cmp1);
        free(cmp2);
        free(cmp3);
    }
    return (0);
}