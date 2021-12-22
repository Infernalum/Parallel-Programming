#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

const long Nstart = 1;
const long Nend = 65000000;

int isPrime(long value)
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
    FILE *fd;
    /* Initialize the MPI */
    ret = MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &procCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int namelen;
    MPI_Get_processor_name(processor_name, &namelen);
    printf("\nПроцесс %d начал работу на компьютере %s\n", rank, processor_name);
    fflush(stdout);

    start = MPI_Wtime();

    long *buf = NULL;

    long wlenght = Nend - Nstart;
    const int wstart = Nstart + (rank * wlenght) / procCount;
    const int wend = Nstart + ((rank + 1) * wlenght) / procCount;

    if (!rank)
    {
        printf("Nstart = %d; Nend = %d", Nstart, Nend);
        fflush(stdout);
        fd = fopen("простые числа.txt", "w");
        MPI_Barrier(MPI_COMM_WORLD);
        buf = calloc(wlenght, sizeof(long));
    }
    else
    {
        MPI_Barrier(MPI_COMM_WORLD);
        buf = calloc(wend - wstart, sizeof(long));
    }

    long count = 0;

    // Проверка каждым процессором своего диапазона
    for (long i = wstart; i < wend; ++i)
        if (isPrime(i))
        {
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
        for (int i = 0; i < wlenght; ++i)
            if (buf[i])
                fprintf(fd, "%d\n", buf[i]);
        free(buf);
    }
    else
    {
        MPI_Send(buf, wend - wstart, MPI_LONG, 0, 0, MPI_COMM_WORLD);
        free(buf);
    }

    end = MPI_Wtime();
    printf("\n wall clock time (proc #%d) = %f\n", rank, end - start);
    ret = MPI_Finalize();
    return 0;
}