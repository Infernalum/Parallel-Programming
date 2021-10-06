#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char **argv)
{
    const int threads = 8;
    int seed = 9154821;
    const long count = 1e8;
    const int accuracy = 10;
    const double frequency = 3.5;
    const int iterations = 100;
    int target = 123456;
    int index = -1;
    double start = 0, end = 0, time = 0;
    double average_time = 0;
    double current_frequency = 0;

    int *array = 0;
    for (int i = 0; i < iterations; ++i)
    {
        //printf("seed: %d;\n", seed);
        srand(seed);
        current_frequency = (rand() % (int)(frequency * 10 + 1 - 0) + 0) / 10.0;
        array = (int *)malloc(count * sizeof(long));
        for (int i = 0; i < count; i++)
            array[i] = rand() % (int)(count / frequency + 1 - 0) + 0;
        for (int j = 0; j < accuracy; ++j)
        {
            start = omp_get_wtime();
            for (int i = 0; i < count; ++i)
                if (array[i] == target)
                {
                    index = i;
                    break;
                }
            end = omp_get_wtime();
            time += (end - start);
        }
        time /= (double)accuracy;
        //printf(" Found occurence of %d at index %d, \ttime: %.8f;\n", target, index, time);
        free(array);
        seed += 10;
        index = -1;
        average_time += time;
        time = 0;
    }
    printf(" Average searching time (consecutive algorithm): %.8f\n", average_time / (double)iterations);

    seed -= (10 * iterations);

    for (int i = 0; i < 10; ++i)
    {
        volatile flag = 0;
        start = omp_get_wtime();
        int t = count / threads;
#pragma omp parallel num_threads(threads) shared(threads, count, flag, t) default(none)
        {
#pragma omp for
            for (i = 0; i < threads; ++i)
            {
                for (int j = i; j < (i + 1) * 1000; ++j)
                {
          
      
                }
            }
        }

        end = omp_get_wtime();
        printf("time: %f\n", end - start);
        flag = 0;
    }

    printf(" Average searching time (parallel algorithm): %.8f\n", average_time / (double)iterations);

    return (0);
}