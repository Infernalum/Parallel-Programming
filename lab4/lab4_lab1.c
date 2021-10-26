#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define _THREADS 2

int main(int argc, char **argv)
{
    const int count = 1e7;
    int random_seed = 92021;
    const int expr = 10;
    int max_1, max_2;
    int ind_1, ind_2;
    double start, end;
    double consecutive_time = 0, parallel_time = 0;

    //printf("OPENMP version: %d\n", _OPENMP);
    //printf("%d\n", omp_get_max_threads());
    printf("count of threads: %d\n", _THREADS);

    for (int k = 0; k < expr; ++k)
    {
        ind_1 = -1;
        ind_2 = -1;
        int *array = NULL;
        srand(random_seed);
        array = (int *)malloc(count * sizeof(int));
        for (int i = 0; i < count; i++)
        {
            array[i] = (rand() % 10 + 1);
        }
        array[100000] = 20; 
        printf("seed: %d:\n", random_seed);
        max_1 = array[0];
        start = omp_get_wtime();
        for (int i = 0; i < count; ++i)
        {
            if (array[i] > max_1)
            {
                max_1 = array[i];
                ind_1 =i;
            };
        };
        end = omp_get_wtime();
        consecutive_time += (end - start);
        for (int j = 0; j < expr; ++j)
        {
            max_2 = array[0];
            ind_2 = -1;
            start = omp_get_wtime();
#pragma omp parallel num_threads(_THREADS) shared(array, count) reduction(max \
                                                                          : max_2) reduction(max : ind_2) default(none)
            {
#pragma omp for schedule(static, _THREADS * _THREADS * _THREADS)
                for (int i = 0; i < count; i++)
                {
                    if (array[i] > max_2)
                    {
                        max_2 = array[i];
                        ind_2 = i;
                    };
                }
            }
            end = omp_get_wtime();
            parallel_time += (end - start);
        }
        random_seed += 10;
        printf("max_1: %d; ind_1: %d;\nmax_2: %d; ind_2: %d;\n", max_1, ind_1, max_2, ind_2);
        free(array);
    }
    printf("Consecutive time: \t%f;\nParallel time: \t\t%f;\n", consecutive_time / (double)expr, parallel_time / ((double)expr * expr));
    return (0);
}