#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#define N 30
#define REPEATS 1000
#define MAX_THREADS 8

typedef struct
{
    double x;
    int start;
    int end;
    double result;
} thread_data;

void *thread_func(void *arg)
{
    thread_data *data = (thread_data *)arg;

    double x = data->x;
    double term = x;
    double sum = 0;

    for (int k = 0; k < data->start; k++)
    {
        term *= -x * x / ((2.0 * k + 2) * (2.0 * k + 3));
    }

    for (int k = data->start; k < data->end; k++)
    {
        sum += term;
        term *= -x * x / ((2.0 * k + 2) * (2.0 * k + 3));
    }

    data->result = sum;

    return NULL;
}

double calculate_sin(double x, int num_threads)
{
    pthread_t threads[MAX_THREADS];
    thread_data data[MAX_THREADS];

    for (int i = 0; i < num_threads; i++)
    {
        data[i].x = x;
        data[i].start = i * N / num_threads;
        data[i].end = (i + 1) * N / num_threads;
        data[i].result = 0;

        if (pthread_create(&threads[i], NULL,
                           &thread_func, &data[i]) != 0)
        {
            fprintf(stderr, "Thread creation error\n");
            exit(1);
        }
    }

    double sum = 0;

    for (int i = 0; i < num_threads; i++)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            fprintf(stderr, "Thread join error\n");
            exit(1);
        }

        sum += data[i].result;
    }

    return sum;
}

int main(void)
{
    double x = 3.0;
    int counts[] = {1, 2, 4, 8};

    double standard = sin(x);

    printf("Standard sin(x) = %.15f\n\n", standard);

    for (int i = 0; i < 4; i++)
    {
        int num_threads = counts[i];

        struct timespec start, end;

        clock_gettime(CLOCK_MONOTONIC, &start);

        double result = 0;

        for (int j = 0; j < REPEATS; j++)
        {
            result = calculate_sin(x, num_threads);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        double elapsed =
            (end.tv_sec - start.tv_sec) +
            (end.tv_nsec - start.tv_nsec) / 1e9;

        double error = fabs(result - standard);

        printf("Threads: %d\n", num_threads);
        printf("Result: %.15f\n", result);
        printf("Error: %.15e\n", error);
        printf("Average time: %.9f s\n\n",
               elapsed / REPEATS);
    }

    return 0;
}