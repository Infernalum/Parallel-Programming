#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>

#define _PHILOSOPHERS 5
#define _MEALS 3
omp_lock_t chop_stick[_PHILOSOPHERS];

void think(int id)
{
    printf("Philosopher #%d is thinking\n", id);
    sleep(rand() % 10 / 2.0);
    printf("Philosopher #%d is hungry\n", id);
}

void eat(int id)
{
    printf("Philosopher #%d is eating\n", id);
    sleep(rand() % 20 / 2.0);
    printf("Philosopher #%d is stuffed\n", id);
}

omp_lock_t *left_chop_stick(int id)
{
    return &chop_stick[(id + 1) % _PHILOSOPHERS];
}

omp_lock_t *right_chop_stick(int id)
{
    return &chop_stick[id];
}

int main()
{
    int i;
    double time;
    for (i = 0; i < _PHILOSOPHERS; i++)
        omp_init_lock(&chop_stick[i]);

#pragma omp parallel num_threads(_PHILOSOPHERS) reduction(max \
                                                          : time)
    {
        double start, end;
        time = 0.0;
        int meals, id = omp_get_thread_num();
        start = omp_get_wtime();
        for (meals = 0; meals < _MEALS; ++meals)
        {
            think(id);
            if (id % 2 == 1)
            {
                omp_set_lock(left_chop_stick(id));
                omp_set_lock(right_chop_stick(id));
            }
            else
            {
                omp_set_lock(right_chop_stick(id));
                omp_set_lock(left_chop_stick(id));
            }
            eat(id);
            omp_unset_lock(left_chop_stick(id));
            omp_unset_lock(right_chop_stick(id));
        }
        end = omp_get_wtime();
        time = end - start;
    }
    printf("Ooh, we thought (and ate) for %.2f minutes!\n", time);
}