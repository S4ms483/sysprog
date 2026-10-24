#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
/* Переделать программу для доказательстава: sin*sin + cos*cos ==1
 * Квадрат синуса считать в одном потоке, косинуса во втором,
 * а результат суммировать в главной программе.*/
void * thread_func1 (void * arg)
{
    double x = *(double *)arg;
    double *result = calloc(1, sizeof(double));

    *result = sin(x) * sin(x);

    return result;
}

void * thread_func2 (void * arg)
{
    double x = *(double *)arg;
    double *result = calloc(1, sizeof(double));

    *result = cos(x) * cos(x);

    return result;
}

int main (void)
{
	pthread_t thread1, thread2;
	double x = 0;
    void *result1, *result2;
	printf("Enter a number: ");
	scanf("%le", &x);

	if (pthread_create (&thread1, NULL,
			                &thread_func1, &x) != 0)
	{
		fprintf (stderr, "Error (thread1)\n");
		return 1;
	}

	if (pthread_create (&thread2, NULL,
			&thread_func2, &x) != 0) 
	{
		fprintf (stderr, "Error (thread2)\n");
		return 1;
	}

    pthread_join(thread1, &result1);
    pthread_join(thread2, &result2);

    double sum = *(double *)result1 + *(double *)result2;

    printf("sin^2(x) = %.10f\n", *(double *)result1);
    printf("cos^2(x) = %.10f\n", *(double *)result2);
    printf("sum = %.10f\n", sum);

    free(result1);
    free(result2);
	
	return 0;
}