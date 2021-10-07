#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>k

int main(int argc, char **argv)
{
    const int threads = 6;
    int seed = 9154821;
    const long count = 1e8;
    const int accuracy = 10;
    const double frequency = 3;
    const int massives = 100;
    int target = 1;
    volatile int index = -1;
    double start = 0, end = 0, _time = 0;
    double consecutive_time = 0, parallel_time = 0;
    double average_time = 0;
    double current_frequency = 0;

    int left = 0, middle = 0, right = 0, empty = 0;

    int *array = 0;
    for (int i = 0; i < massives; ++i)
    {
        printf("seed: %d;\n", seed);
        srand(seed);
        current_frequency = (rand() % (int)(frequency * 10 + 1 - 0) + 0) / 10.0;
        array = (int *)malloc(count * sizeof(int));
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
            _time += (end - start);
        }
        _time /= (double)accuracy;
        consecutive_time += _time;

        if (index == -1)
            ++empty;
        else if (index < count / 3)
            ++left;
        else if (index > 2 * count / 3)
            ++middle;
        else
            ++right;
        printf(" Found occurence of %d at index %d;\n", target, index);
        printf(" Average searching time (consecutive algorithm): \t%.8f\n", _time);

        _time = 0;
        index = -1;

        for (int j = 0; j < accuracy; ++j)
        {
            volatile int flag = 0;
            int p;
            start = omp_get_wtime();
#pragma omp parallel num_threads(threads) shared(array, count, threads, flag, target, index) default(none)
            {
#pragma omp for
                for (p = 0; p < threads; p++)
                {
                    //for (int j = p * (count / threads); j < (p + 1) * (count / threads); ++j)
                    for (int j = p; j < count; j = j + threads)
                    {
                        if (flag == 1)
                            break;
                        if (array[j] == target)
                        {
                            index = j;
                            flag = 1;
                        }
                    }
                }
            }

            end = omp_get_wtime();
            _time += (end - start);
        }
        _time /= (double)accuracy;
        parallel_time += _time;
        printf(" Found occurence of %d at index %d;\n", target, index);
        printf(" Average searching time (parallel algorithm): \t\t%.8f\n", _time);
        seed += 1000;
        free(array);
    }

    printf(" Statistics by the location of the found element:\n empty - %d; left - %d; middle - %d; right - %d;\n", empty, left, middle, right);
    printf(" Average searching time (consecutive algorithm): \t%.8f\n", consecutive_time / (double)massives);
    printf(" Average searching time (parallel algorithm): \t\t%.8f\n", parallel_time / (double)massives);
    return (0);
}