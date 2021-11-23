#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>

int main(int argc, char **argv)
{
	int ret = -1;  ///< For return values
	int size = -1; ///< Total number of processors
	int rank = -1; ///< This processor's number

	const int count = 1e8;			///< Number of array elements
	const int random_seed = 920215; ///< RNG seed

	int *array = 0; ///< The array we need to find the max in
	int lmax = -1;	///< Local xmaximums
	int max = -1;	///< The maximal element
	double start, end;

	start = omp_get_wtime(); // 3 вариант

	/* Initialize the MPI */
	ret = MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	printf("MPI Comm Size: %d;\n", size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	printf("MPI Comm Rank: %d;\n", rank);

	array = (int *)malloc(count * sizeof(int));

	/* Master generates the array */
	//if (!rank)
	{
		srand(random_seed);
		for (int i = 0; i < count; i++)
		{
			array[i] = rand();
		}
	}

	//start = omp_get_wtime(); // 1 вариант

	/* Send the array to all other processors */
	//MPI_Bcast(array, count, MPI_INTEGER, 0, MPI_COMM_WORLD);

	//start = omp_get_wtime(); // 2 вариант
	const int wstart = (rank)*count / size;
	const int wend = (rank + 1) * count / size;

	for (int i = wstart; i < wend; i++)
	{
		if (array[i] > lmax)
		{
			if (array[i] > lmax)
			{
				lmax = array[i];
			}
		}

		MPI_Reduce(&lmax, &max, 1, MPI_INTEGER, MPI_MAX, 0, MPI_COMM_WORLD);

		//end = omp_get_wtime(); // 2 вариант

		ret = MPI_Finalize();
		end = omp_get_wtime(); // 1 вариант

		printf("time: %f\n", end - start);

		return (0);
	}