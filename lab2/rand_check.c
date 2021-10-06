#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
    const long count = 1e6;
    const double frequency = 2.6;
    const int iterations = 1000;
    int target = 12345;
    int count_target = 0;
    int *array = 0;

    int seed = time(NULL);
    for (int k = 0; k < iterations; ++k)
    {
        seed += 10;
        //printf("seed: %d;\n", seed);
        srand(seed);
        array = (int *)malloc(count * sizeof(long));
        for (int i = 0; i < count; i++)
            array[i] = rand() % (int)(count/frequency + 1 - 0) + 0;
            
        for (int i = 0; i < count; ++i)
            if (array[i] == target)
                ++count_target;

        free(array);
    }
    printf("found coincidences: %f\n", count_target/(double)iterations);
    return (0);
}