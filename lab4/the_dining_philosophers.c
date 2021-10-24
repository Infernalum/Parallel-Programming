#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>

#define _PHILOSOPHERS 5
#define _MEALS 3
omp_lock_t chop_stick[_PHILOSOPHERS];

void think(int id, double *time)
{
    printf("Philosopher #%d is thinking\n", id);
    double sl = rand() % 10 / 2.0;
    sleep(sl);
    (*time) += sl;
    printf("Philosopher #%d is hungry\n", id);
}

void eat(int id, double *time)
{
    printf("Philosopher #%d is eating\n", id);
    double sl = rand() % 20 / 2.0;
    sleep(sl);
    (*time) += sl;
    printf("Philosopher #%d is stuffed\n", id);
}

omp_lock_t *left_chop_stick(int id)
{
    return &chop_stick[(id - 1 + _PHILOSOPHERS) % _PHILOSOPHERS];
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
        time = 0.0;
        int meals, id = omp_get_thread_num();
        for (meals = 0; meals < _MEALS; ++meals)
        {
            think(id, &time);
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
            eat(id, &time);
            omp_unset_lock(left_chop_stick(id));
            omp_unset_lock(right_chop_stick(id));
        }
    }
    printf("Ooh, we thought (and ate) for %.2f minutes!\n", time);
}