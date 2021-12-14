#include <math.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>

int isSimple(long value)
{
    int limit = floor(sqrt(value));
    int i = 1;
    int real_i = 1;
    while (real_i <= limit)
    {
        real_i = i * 6;
        if (!(value % (real_i - 1)))
            return 0;
        if (!(value % (real_i + 1)))
            return 0;
        ++i;
    }
    return 1;
}

int main(int argc, char **argv)
{
    int ret = -1;       ///< For return values
    int procCount = -1; ///< Total number of processors
    int rank = -1;      ///< This processor's number
    int Nstart = 1e6;
    int Nend = 1e7;
    double start, end;

    /* Initialize the MPI */
    ret = MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &procCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (!rank)
        printf("MPI Comm Size: %d;\n", procCount);

    printf("MPI Comm Rank: %d;\n", rank);

    start = omp_get_wtime(); // 1 вариант

    const int wstart = Nstart + (rank * (Nend - Nstart)) / procCount;
    const int wend = Nstart + ((rank + 1) * (Nend - Nstart)) / procCount;
    const int lenght = wend - wstart;

    long long count = 0;
    start = omp_get_wtime();
    for (long i = wstart; i < wend; ++i)
        if (isSimple(i))
            ++count;
    end = omp_get_wtime();
    printf("%lu; time: %.8f;\n", count, end - start);
    ret = MPI_Finalize();
    end = omp_get_wtime();

    return (0);
}