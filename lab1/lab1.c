#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

int main(int argc, char** argv) {
	const int count = 1e8;
	const int threads = 8;
	int random_seed = 920215;
	const int expr = 10;
	int max;
	double start, end;

	//printf("OPENMP version: %d\n", _OPENMP);
	printf("count of threads: %d\n", threads);

	for (int k = 0; k < expr; ++k) {
		int* array = NULL;
		srand(random_seed);
		array = (int*)malloc(count * sizeof(int));
		for (int i = 0; i < count; i++) { array[i] = (rand() * rand()); }
		printf("seed: %d:\n", random_seed);
		max = array[0];
		start = omp_get_wtime();
		for (int i = 0; i < count; ++i) {
			if (array[i] > max) { max = array[i]; };
		};
		end = omp_get_wtime();
		printf("Max element for consecutive function: %d, time: %f;\n", max, end - start);

		double time = 0, all_time = 0;

		for (int j = 0; j < expr; ++j) {
			max = array[0];
			time = 0;
			int i;
			start = omp_get_wtime();
#pragma omp parallel num_threads(threads) shared(array, count) reduction(max: max) default(none)
			{
#pragma omp for
				for (i = 0; i < count; i++)
				{
					if (array[i] > max) { max = array[i]; };
				}
			}
			end = omp_get_wtime();
			time = end - start;
			all_time += time;
		}
		printf("Max element for parallel function:    %d, time: %f;\n", max, all_time / (double)expr);
		random_seed += 10;
		max = array[0];
		free(array);
	}
	return(0);
}