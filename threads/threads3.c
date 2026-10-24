#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <bits/time.h>


#define N 10
/* Модифицировать программу, 
 * чтобы замерить среднее время завершения нити после сигнала на завершение.
 */
void * any_func (void * arg)
{
  while (1) 
  {
  	fprintf (stderr, ".");
  	sleep (10);
  }
  
  return NULL;
}

int main (void)
{
  pthread_t thread;
  void * result;

  struct timespec start, end;
  double total_time = 0;

  for (int i = 0; i < N; i++) {
      if (pthread_create (&thread, NULL, &any_func, NULL) != 0) 
      {
          fprintf (stderr, "Error\n");
          return 1;
      }
      sleep (5);
      clock_gettime(CLOCK_MONOTONIC, &start);
      pthread_cancel (thread);
      if (!pthread_equal (pthread_self (), thread))
      {
        pthread_join (thread, &result);
      }
    
      clock_gettime(CLOCK_MONOTONIC, &end);
    
      double elapsed = (end.tv_sec - start.tv_sec) +
                        (end.tv_nsec - start.tv_nsec) / 1e9;
    
      total_time += elapsed;
    
      printf("Time %d: %.9f s\n", i + 1, elapsed);
      if (result == PTHREAD_CANCELED)
      {
          fprintf (stderr, "Canceled\n");
      }
  }

  printf("Average time: %.9f s\n", total_time / N);
  return 0;
}