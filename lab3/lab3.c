#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int ifSort(int *array, int size)
{
    for (int i = 0; i < size - 1; ++i)
        if (array[i] > array[i + 1])
            return 0;
    return 1;
}

void InsertionSort(int *array, int size)
{
    int *object = (int *)malloc(sizeof(int) * size);
    for (int i = 0; i < size; ++i)
        object[i] = array[i];
    double start, end;
    int iter = 0;

    /* Sorting */
    start = omp_get_wtime();
    for (int i = 1; i < size; ++i)
    {
        int key = object[i];
        int j = i - 1;
        while (j >= 0 && object[j] > key)
        {
            object[j + 1] = object[j];
            --j;
            ++iter;
        }
        object[j + 1] = key;
    }
    end = omp_get_wtime();
    /* Show sorted array */
    //for (int i = 0; i < size; ++i)
    //    printf("%d, ", object   [i]);
    printf("Insertion Sort time: \t\t%.8f, totally iterations: \t%d;\n", end - start, iter);
    printf("%d\n", ifSort(object, size));
    free(object);
}

void ShellSort(int *array, int size)
{
    int *object = (int *)malloc(sizeof(int) * size);
    for (int i = 0; i < size; ++i)
        object[i] = array[i];
    double start, end;
    int iter = 0;
    start = omp_get_wtime();
    int tmp;

    for (int step = size / 2; step > 0; step /= 2)
    {
        for (int i = 0; i < size; ++i)
        {
            for (int j = i + step; j < size; j += step)
            {
                tmp = object[i];
                if (tmp > object[j])
                {
                    object[i] = object[j];
                    object[j] = tmp;
                    ++iter;
                    //printf("Поток 0 меняет местами элементы %d и %d;\n", i, j);
                }
            }
        }
    }
    end = omp_get_wtime();
    /* Show sorted array */
    //for (int i = 0; i < size; ++i)
    //    printf("%d, ", object[i]);
    //printf("\n");
    printf("Shell Sort time: \t\t%.8f, totally itera tions: \t%d;\n", end - start, iter);
    printf("%d\n", ifSort(object, size));
    free(object);
}

void ShellParallelSort(int *array, int size, int threads)
{
    int *object = (int *)malloc(sizeof(int) * size);
    for (int i = 0; i < size; ++i)
        object[i] = array[i];
    double start, end;
    start = omp_get_wtime();

#pragma omp parallel num_threads(threads) shared(object), firstprivate(size)
    {
        int m = omp_get_thread_num();
        for (int step = size / 2; step > 0; step /= 2)
        {
            int N = (size - step) / threads;
#pragma omp parallel for ordered
            for (int i = 0; i < threads; ++i)
            {
                for (int p = i * N; p < (i + 1) * N; ++p)
                {
#pragma omp ordered
                    {
                        int j = p;
                        while (j >= 0 && object[j] > object[j + step])
                        {
                            int tmp = object[j];
                            object[j] = object[j + step];
                            object[j + step] = tmp;
                            //printf("Поток %d меняет местами элементы %d и %d;\n", m, j, j + step);
                            --j;
                        }
                    }
                }
            }
        }
    }

    end = omp_get_wtime();
    /* Show sorted array */
    //for (int i = 0; i < size; ++i)
    //    printf("%d, ", object[i]);
    //printf("\n");
    printf("Shell Parallel Sort time: \t%.8f;\n", end - start);
    printf("%d\n", ifSort(object, size));
    free(object);
}

int main(int argc, char **argv)
{
    const int threads = 2;
    int seed = 915482;
    const long count = 1e4;
    const int accuracy = 10;
    const double frequency = 0.1;
    const int massives = 1;
    volatile int index = -1;
    double start = 0, end = 0, _time = 0;
    double consecutive_time = 0, parallel_time = 0;
    double average_time = 0;

    int *array = 0;
    for (int i = 0; i < massives; ++i)
    {
        printf("seed: %d;\n", seed);
        srand(seed);
        array = (int *)malloc(count * sizeof(int));
        for (int i = 0; i < count; i++)
            array[i] = rand() % (int)(count / 0.1 + 1 - 0) + 0;
        //for (int i = 0; i < count; ++i)
        //     printf("%d, ", array[i]);
        //   printf("\n");
        InsertionSort(array, count);
        ShellSort(array, count);
        ShellParallelSort(array, count, threads);
        for (int j = 0; j < accuracy; ++j)
        {
        }

        index = -1;

        for (int j = 0; j < accuracy; ++j)
        {
        }

        seed += 1000;
        free(array);
    }

    printf(" Average searching time (consecutive algorithm): \t%.8f\n", consecutive_time / (double)massives);
    printf(" Average searching time (parallel algorithm): \t\t%.8f\n", parallel_time / (double)massives);
    return (0);
}