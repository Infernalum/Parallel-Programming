#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int *chisla, *chisla_buf;
    int myrank, size; // Номер текущего процесса и общее число процессов
    int namelen, num_proc;
    int TAG = 0;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    double startwtime, endwtime;
    FILE *prchisla;
    int fl;
    char ch[50];
    int porog, diapazon, ost, start, end;
    int i, j, flag;
    int buf, kol;

    MPI_Init(&argc, &argv); // Инициализируем работу программы MPI
    // Инициализируем счетчик отсчета времени работы программы
    // Определяем номер процесса
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    // Определяем общее число процессоров
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // Определяем имя компьютера, на котором запущен процесс
    MPI_Get_processor_name(processor_name, &namelen);
    // Выводим полученную информацию на экран
    fprintf(stderr, "\nПроцесс %d начал работу на компьютере %s\n", myrank, processor_name);
    fflush(stderr);
    if (myrank == 0) // Если это главный процессор, то
    {
        // Открываем запись в файл, в который будем записывать найденные простые числа
        prchisla = fopen("простые числа.txt", "w");
        // Вводим число, в пределах которого необходимо будет найти простые числа
        printf("\n Введите диапазон для определения простых чисел: ");
        fflush(stdout);
        do
        {
            gets(ch); // считываем вводимое число посимвольно
            fl = 1;
            for (i = 0; i < strlen(ch); ++i)
            {
                if ((ch[i] < '0') || (ch[i] > '9')) // и проверяем каждый символ
                    fl = 0;
            }
            if (fl == 0) // Если хотя бы один из исмволов введен неверно, то выводим ссотвествующее сообщение об ошибке
            {
                printf("\n Ошибка. Попробуйте еще раз.");
                fflush(stdout);
                // Повторяем попыткку ввода
                printf("\n Введите диапазон для определения простых чисел:");
                fflush(stdout);
            }
        } while (fl == 0);
        porog = atoi(ch);              // Преобразуем введенное число
        startwtime = MPI_Wtime();      // Инициализиуем счетчик отсчета времени расчетов
        diapazon = porog / size;       // Количество чисел для анализа каждым процессом
        ost = porog - diapazon * size; // Отстаток от деления
        // Гравный процесс посылает всем остальным процессам диапазон анализа
        MPI_Bcast(&diapazon, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        // Ожидаем, пока все процессы получат данные
        start = (size - 1) * diapazon + 2; // Определяем начально число анализа
        end = (size * diapazon) + ost;     // Определяем конечное число анализа
        chisla = (int *)malloc((diapazon + ost) * sizeof(int));
        // Инициализируем массивы для хранения результатов анализа
        chisla_buf = (int *)malloc(diapazon * sizeof(int));
        for (i = 0; i < (diapazon + ost); ++i)
            chisla[i] = 0; // Обгуляем иниицализированные массивы
        for (i = 0; i < diapazon; ++i)
            chisla_buf[i] = 0;
        kol = 0;
        if (start == 2) // Если начальное число анализа равно 2
        {
            chisla[kol] = 2; // То заносим его в массив результата
            ++kol;
            start++;
        }                              // И преходим к дальнейшему анализу
        for (i = start; i <= end; ++i) // Поочередно берем каждый из элементов
        {
            j = 1;
            flag = 1; // устанавливаем флаг в единицу
            do
            {
                j++;
                // Начинаем делить на все возможные делители
                // Первый раз j = 2;
                buf = i / j;
                if ((buf * j) == i) // Если число поделилось нацело, то оно не является простым
                    flag = 0;       // И поэтому флаг устанавливается в 0
            }
            // Анализ продолжается, пока не будут проверены все делители, и флаг не будет равен единице
            while ((j < (i - 1)) && (flag == 1));
            if (flag == 1) // Если флаг остался равен единице
            {
                // То это простое число, и мы заносим его в массив
                chisla[kol] = j + 1;
                ++kol;
            }
        }
        if (size > 1) // Если в вычислениях участвует больше одного процесса
        {
            for (j = 1; j < size; ++j)
            {
                // То принимает данные от каждого из них
                num_proc = j;
                MPI_Status status;
                MPI_Recv(chisla_buf, diapazon, MPI_INT, num_proc, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                i = 0;

                while ((i < diapazon) && (chisla_buf[i] != 0))
                {
                    if (chisla_buf[i] != 0) // Заносим принятые данные в выходной файл
                    {
                        fprintf(prchisla, "%d \n", chisla_buf[i]);
                        fflush(prchisla);
                    }
                    ++i;
                }
            }
        }
        i = 0; // После этого заносим в выходной файл данные, полученные главным процессором
        while ((i < (diapazon + ost)) && (chisla[i] != 0))
        {
            if (chisla[i] != 0)
            {
                fprintf(prchisla, "%d \n", chisla[i]);
                fflush(prchisla);
            }
            ++i;
        }
        MPI_Barrier(MPI_COMM_WORLD);
        // Ожидаем, пока все процессы вызовут эту функцию
        endwtime = MPI_Wtime(); // Выводим на экран
        printf("\n wall clock time = %f\n", endwtime - startwtime);
        // Время работы программы
        fflush(stdout);
        printf("\n Анализ завершен!\n");
        fflush(stdout);
        free(chisla); // Освобождаем динамическую память
        free(chisla_buf);
        fclose(prchisla); // Закрываем выходной файл
    }
    else // Если это не главный процесс
    {
        // то принимаем данные от главного процесса
        MPI_Bcast(&diapazon, 1, MPI_INT, 0, MPI_COMM_WORLD);
        printf("\n Процесс %d получил данные от главного процесса, diapazon = %d\n", myrank, diapazon);
        fflush(stdout);

        MPI_Barrier(MPI_COMM_WORLD);         // Ожидаем, пока все процессы вызовут эту функцию
        start = (myrank - 1) * diapazon + 2; // Определяем начальное число анализа
        end = (myrank * diapazon) + 1;       // Определяем число анализа
        // Инициализируем массив для хранения результатов анализа
        chisla = (int *)malloc(diapazon * sizeof(int));
        for (i = 0; i < (diapazon); ++i)
            chisla[i] = 0; // Обгуляем инициализированные массивы
        kol = 0;
        if (start == 2) // Если начальное число анализа равно 2
        {
            chisla[kol] = 2; // То заносим его  в массив результата
            kol++;           // И переходим к дальнейшему анализу
            start++;
        }
        for (i = start; i <= end; ++i) // Поочередно берем каждый из элементов
        {
            j = 1;
            flag = 1; // Устанавливаем флаг в единицу
            do
            {
                ++j; // Начинаем делить на все возможные делители
                // Первый раз j = 2
                buf = i / j;
                if ((buf * j) == i) // Если число поделилось нацело, то оно является простым
                    flag = 0;       // И поэтому флаг устанавливается в 0
            }
            // Анализ продолжается, пока не буду проверены все делители и флаг не будет равен единице
            while ((j < (i - 1)) && (flag == 1));
            if (flag == 1) // Если флаг остался равен единице
            {
                // То это простое число и мы заносим его в массив
                chisla[kol] = j + 1;
                ++kol;
            }
        }
        // Передаем массив с результатами анализа главному процессору
        MPI_Send(chisla, diapazon, MPI_INT, 0, TAG, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        // Ожидаем, пока все процессы вызовут эту функцию
        free(chisla); // Освобождаем динамическую память
    }
    MPI_Finalize(); // Завершаем работу программы MPI
    return 0;
}
