#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>

const long Nstart = 2;
const long Nend = 1e6;

int isSimple(long value)
{
    int limit = floor(sqrt(value));
    for (int i = 2; i <= limit; ++i)
    {
        if (!(value % i))
            return 0;
    }
    return 1;
}

int main(int argc, char **argv)
{
    int ret = -1;       ///< For return values
    int procCount = -1; ///< Total number of processors
    int rank = -1;      ///< This processor's number
    double start, end;

    /* Initialize the MPI */
    ret = MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &procCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    start = omp_get_wtime();

    long *buf = NULL;
    long wlenght = Nend - Nstart;
    const int wstart = Nstart + (rank * wlenght) / procCount;
    const int wend = Nstart + ((rank + 1) * wlenght) / procCount;

    if (!rank)
    {
        printf("MPI Comm Size: %d;\n", procCount);
        buf = calloc(wlenght, sizeof(long));
    }
    else
    {
        buf = calloc(wend - wstart, sizeof(long));
    }

    long count = 0;

    for (long i = wstart; i < wend; ++i)
        if (isSimple(i))
        {
            //printf("Rank: %d; num_thread: %d; чиселко: %d;\n", rank, omp_get_thread_num(), i);
            ++count;
            buf[i - wstart] = i;
        }

    if (!rank)
    {
        for (int i = 1; i < procCount; ++i)
        {
            const int mpiStart = (i * wlenght) / procCount;
            const int mpiEnd = (i + 1) * wlenght / procCount;
            MPI_Recv(buf + mpiStart, mpiEnd - mpiStart, MPI_LONG, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        //int c = 0;
        //for (int k = 0; k < wlenght; ++k)
        //    if (buf[k] != 0)
        //        ++c;
        //printf("%d - summ\n", c);
        for (int i = 0; i < wlenght; ++i)
            if (buf[i])
                printf("%d\n", buf[i]);
        free(buf);
    }
    else
    {
        MPI_Send(buf, wend - wstart, MPI_LONG, 0, 0, MPI_COMM_WORLD);
        free(buf);
    }

    ret = MPI_Finalize();
    end = omp_get_wtime();
    printf("%lu; time: %.8f;\n", count, end - start);

    return (0);
}