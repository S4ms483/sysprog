/* разобраться как работает, написать комментарии,
   в том числе ко всем параметрам. */
#include <sys/shm.h>
#include <stdio.h>

int main (int argc, char ** argv)
{
  int shm_id;
  char * shm_buf;
  
  if (argc < 2) // смотрим количество аргументов
  {
  	fprintf (stderr, "Too few arguments\n");
  	return 1;
  }
  
  shm_id = atoi(argv[1]); // вторым должен быть id процесса из предыдущего примера
  shm_buf = (char *) shmat (shm_id, 0, 0); // подключаемся к переданному проекту
  if (shm_buf == (char *) (-1)) 
  {
  	fprintf (stderr, "shmat() error\n");
  	return 1;
  }
  
  printf ("Message: %s\n", shm_buf); // читаем содержимое этого сегмента
  shmdt (shm_buf);
  
  return 0;
}