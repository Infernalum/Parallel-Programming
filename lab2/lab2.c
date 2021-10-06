#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char **argv)
{
    const int threads = 2;
    int seed = 9154821;
    const long count = 1e8;
    const int accuracy = 10;
    const double frequency = 1;
    const int massives = 100;
    int target = 1;
    int index = -1;
    double start = 0, end = 0, time = 0;
    double average_time = 0;
    double current_frequency = 0;

    int left = 0, middle = 0, right = 0, empty = 0;

    int *array = 0;
    for (int i = 0; i < massives; ++i)
    {
        //printf("seed: %d;\n", seed);
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
            time += (end - start);
        }
        time /= (double)accuracy;
        if (index == -1)
            ++empty;
        else if (index < count / 3)
            ++left;
        else if (index > 2 * count / 3)
            ++middle;
        else
            ++right;
        printf(" Found occurence of %d at index %d, \ttime: %.8f;\n", target, index, time);
        free(array);
        seed += 10;
        index = -1;
        average_time += time;
        time = 0;
    }
    printf(" Statistics by the location of the found element:\n empty - %d; left - %d; middle - %d; right - %d;\n", empty, left, middle, right);
    printf(" Average searching time (consecutive algorithm): \t%.8f\n", average_time / (double)massives);

    seed -= (accuracy * massives);
    time = average_time = 0;
    for (int i = 0; i < massives; ++i)
    {
        srand(seed);
        current_frequency = (rand() % (int)(frequency * 10 + 1 - 0) + 0) / 10.0;
        array = (int *)malloc(count * sizeof(int));
        for (int i = 0; i < count; i++)
            array[i] = rand() % (int)(count / frequency + 1 - 0) + 0;
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
                    for (int j = p * (count / threads); j < (p + 1) * (count / threads); ++j)
                    //for (int j = p; j < count; j = j + threads)
                    {
                        if (flag == 1)
                            break;
                        if (array[j] == target)
                        {
                            index = j;
                            flag = 1;
                            break;
                        }
                    }
                }
            }

            end = omp_get_wtime();
            time += (end - start);
        }
        seed += 10;
        free(array);
    }
    time /= (double)accuracy;
    printf(" Average searching time (parallel algorithm): \t\t%.8f\n", time / (double)massives);

    return (0);
}