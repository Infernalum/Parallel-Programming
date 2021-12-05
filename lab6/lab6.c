#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>

int size = 1500000;

#define RAND rand() % (size + 1 - 0) + 0

int *ShellSort(int *beginIt, int *endIt)
{
    int size = endIt - beginIt;

    for (int step = size / 2; step > 0; step /= 2)
    {
        for (int i = 0; i < step; ++i)
        {
            for (int j = i + step; j < size; j += step)
            {
                int key = beginIt[j], p = j - step;
                {
                    while (p >= 0 && beginIt[p] > key)
                    {
                        beginIt[p + step] = beginIt[p];
                        p -= step;
                    }
                    beginIt[p + step] = key;
                }
            }
        }
    }
    return beginIt;
}

int _max(int *cur, int size, int *curind)
{
    int max = cur[0];
    for (int i = 0; i < size; + i)
        if (max < cur[i])
        {
            max = cur[i];
            (*curind) = i;
        }
    return max;
}

int *mergeVectors(int *indexes, int procCount, int *oldArray)
{
    int *object = (int *)malloc(sizeof(int) * size);
    for (int i = 0; i < size; ++i)
    {
        int curind = -1;
        int *cur = (int *)malloc(sizeof(int) * procCount);
        for (int j = 0; j < procCount; ++j)
            cur[j] = oldArray[indexes[procCount]];
        object[i] = _max(cur, procCount, &curind);
        indexes[curind]++;
        free(cur);
    }
}

int *merge(int *s, int low, int up, int l)
{
    int *b, *c, v;
    int i, j, k;
    b = calloc(l, sizeof(int));
    c = calloc(up + 1 - l, sizeof(int));
    for (int i = 0; i < low + l; ++i)
        b[i - low] = s[i];
    for (int i = 0; i < up - l; ++i)
        c[i] = s[i + l + low];
    v = (b[l] = (c[up - l] = (s[low + l - 1] > s[up - 1]) ? (s[low + l - 1] + 1) : (s[up - 1] + 1)));
    i = (j = 0);
    k = low;
    while (b[i] < v || c[j] < v)
    {
        if (b[i] < c[j])
            s[k] = b[i++];
        else
            s[k] = c[j++];
        k++;
    }
    return s;
}

int isSort(int *array)
{
    for (int i = 0; i < size - 1; ++i)
        if (array[i] > array[i + 1])
            return 0;
    return 1;
}

int main(int argc, char **argv)
{
    int ret = -1;       ///< For return values
    int procCount = -1; ///< Total number of processors
    int rank = -1;      ///< This processor's number

    const int random_seed = 666666; ///< RNG seed

    double start, end;

    /* Initialize the MPI */
    ret = MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &procCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (!rank)
        printf("MPI Comm Size: %d;\n", procCount);

    printf("MPI Comm Rank: %d;\n", rank);

    int *array = (int *)malloc(size * sizeof(int));

    /* Master generates the array */
    if (!rank)
    {
        srand(random_seed);
        for (int i = 0; i < size; i++)
            array[i] = RAND;
    }

    //for (int i = 0; i < count; ++i)
    //    printf("%d, ", array[i]);
    //printf("\n");

    start = omp_get_wtime(); // 1 вариант

    const int wstart = (rank)*size / procCount;
    const int wend = (rank + 1) * size / procCount;
    const int lenght = wend - wstart;

    /* Send the array parts to all other processors */
    if (!rank)
    {
        for (int i = 1; i < procCount; ++i)
        {
            const int mpiStart = (i * size) / procCount;
            const int mpiEnd = (i + 1) * size / procCount;
            const int mpiLenght = mpiEnd - mpiStart;
            MPI_Send(array + mpiStart, mpiLenght, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        ShellSort(array, array + wend);
        for (int i = 1; i < procCount; ++i)
        {
            const int mpiStart = (i * size) / procCount;
            const int mpiEnd = (i + 1) * size / procCount;
            const int mpiLenght = mpiEnd - mpiStart;
            MPI_Recv(array + mpiStart, mpiLenght, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        //for (int j = 0; j < size; ++j)
        //    printf("%d, ", array[j]);
    }
    else
    {
        int *buf = (int *)malloc(lenght * sizeof(int));
        MPI_Recv(buf, lenght, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        ShellSort(buf, buf + lenght);
        MPI_Send(buf, lenght, MPI_INT, 0, 0, MPI_COMM_WORLD);
        free(buf);
    }

    ret = MPI_Finalize();
    end = omp_get_wtime();

    //for (int i = 0; i < count; ++i)
    //    printf("%d, ", array[i]);
    printf("\n");
    //printf("IsSort: %d\n", isSort(array));
    printf("time: %f\n", end - start);

    return (0);
}