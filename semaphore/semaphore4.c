#include <sys/shm.h>
#include <stdio.h>
#include <sys/sem.h>

#define SHMEM_SIZE 4096

#define SEM_KEY 2007 //даем те же ключи сегмента и семафора чтобы
#define SHM_KEY 2007 //можно было работать в паре с предыдущей программой

int main (int argc, char ** argv)
{
	int shm_id, sem_id;
	char * shm_buf;
	struct sembuf sb[1];

	shm_id = shmget (SHM_KEY, SHMEM_SIZE, 0600); //сегмент уже не создаем а просто к нему подключаемся
	if (shm_id == -1) 
	{
		fprintf (stderr, "shmget() error\n");
		return 1;
	}
	
	sem_id = semget (SEM_KEY, 1, 0600); //подключаемся к одному семафору
	if (sem_id == -1) 
	{
		fprintf (stderr, "semget() error\n");
		return 1;
	}

	shm_buf = (char *) shmat (shm_id, 0, 0);
	if (shm_buf == (char *) -1) 
	{
		fprintf (stderr, "shmat() error\n");
		return 1;
	}

	printf ("Message: %s\n", shm_buf); //читаем строку из shmem

	sb[0].sem_num = 0;
	sb[0].sem_flg = SEM_UNDO;

	sb[0].sem_op = 1; //увеличиваем семафор на 1 чтобы первая программ
	semop (sem_id, sb, 1); //могла сделать -1 второй раз
	
	shmdt (shm_buf); //отключаемся от shmem

	return 0;
}