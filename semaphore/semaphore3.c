#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHMEM_SIZE	4096 //размер общей памяти
#define SH_MESSAGE	"Hello World!\n" //сообщение которое мы туда напишем

#define SEM_KEY		2007
#define SHM_KEY		2007

union semnum 
{
	int val;
	struct semid_ds * buf;
	unsigned short * array;
} sem_arg;


int main (void)
{
	int shm_id, sem_id; //id сегмента и семафора
	char * shm_buf; //указатель на сегмент
	int shm_size; //его размер
	struct shmid_ds ds; //информация о сегменте
	struct sembuf sb[1]; //массив из одной операции над семафором
	unsigned short sem_vals[1]; //начальное значение семафора
	
	shm_id = shmget (SHM_KEY, SHMEM_SIZE,
			IPC_CREAT | IPC_EXCL | 0600); //создаем сегмент, даем все права только владельцу

	if (shm_id == -1) 
	{
		fprintf (stderr, "shmget() error\n");
		return 1;
	}

	sem_id = semget (SEM_KEY, 1, //создаем 1 семафор
			0600 | IPC_CREAT | IPC_EXCL);

	if (sem_id == -1)
	{
		fprintf (stderr, "semget() error\n");
		return 1;
	}

	printf ("Semaphore: %d\n", sem_id); 
	sem_vals[0] = 1; //значением семафора делаем 1
	sem_arg.array = sem_vals;

	if (semctl (sem_id, 0, SETALL, sem_arg) == -1)
	{
		fprintf (stderr, "semctl() error\n");
		return 1;
	}
 
	shm_buf = (char *) shmat (shm_id, NULL, 0); //подключаем сегмент к адресному пространству процесса
	if (shm_buf == (char *) -1)
	{
		fprintf (stderr, "shmat() error\n");
		return 1;
	}

	shmctl (shm_id, IPC_STAT, &ds);
	
	shm_size = ds.shm_segsz;
	if (shm_size < strlen (SH_MESSAGE)) //проверяем что сообщение записалось целиком
	{
		fprintf (stderr, "error: segsize=%d\n", shm_size);
		return 1;
	}
	
	strcpy (shm_buf, SH_MESSAGE); //был неправильный порядок аргументов

	printf ("ID: %d\n", shm_id);
	
	sb[0].sem_num = 0;
	sb[0].sem_flg = SEM_UNDO; //разрешвем откатывать изменения в случае ошибки

	sb[0].sem_op = -1; //уменьшаем семафор на 1
	semop (sem_id, sb, 1); //и сразу выполняем

	sb[0].sem_op = -1; 
	semop (sem_id, sb, 1); //а потом пробуем еще раз, но семафор отрицательным быть не может

	semctl (sem_id, 0, IPC_RMID, sem_arg); //удаляем набор семафоров
	shmdt (shm_buf); // отключаемся от сегмента
	shmctl (shm_id, IPC_RMID, NULL);  // и удаляем его

	return 0;
}