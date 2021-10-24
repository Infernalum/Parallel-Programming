#include "stdio.h"
#include "stdlib.h"
#include <time.h>
#include <omp.h>

int main(int argc, int **argv)
{
    int val;
#ifdef _OPENMP
    printf("Версия OpenMP: %d\n", _OPENMP);
#endif
    printf("Число доступных процессоров: %d;\n", omp_get_num_procs());
    printf("Число доступных потоков: %d;\n", omp_get_num_threads());
    printf("Доступность динамического изменения нитей: ");
    switch (val = omp_get_dynamic())
    {
    case 1:
        printf("on;\n");
        break;
    default:
        printf("off;\n");
        break;
    }
    printf("Разрешение таймера: %.11f сек.;\n", omp_get_wtick());
    printf("Настройки вложенного параллелилизма: ");
    switch (val = omp_get_nested())
    {
    case 1:
        printf("on;\n");
        break;
    default:
        printf("off;\n");
        break;
    }
    printf("Максимально возможная глубина вложеннего параллелизма: %d;\n", omp_get_max_active_levels());
    omp_sched_t kind;
    int chunk_size;
    omp_get_schedule(&kind, &chunk_size);
    printf("Текущие настройки распределения итерационной нагрузки: ");
    switch (kind)
    {
    case omp_sched_static:
        printf("static");
        break;
    case omp_sched_dynamic:
        printf("dynamic");
        break;
    case omp_sched_guided:
        printf("guided");
        break;
    case omp_sched_auto:
        printf("auto");
        break;
    default:
        printf("other (implementation specific)");
        break;
    }
    printf(";\nРазмер чанка: %d;\n", chunk_size, (chunk_size > 1) ? "s" : "");
    return 0;
}