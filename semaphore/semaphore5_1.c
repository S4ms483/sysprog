#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define SHMEM_SIZE	512 //размер сделаем поменьше

#define SEM_KEY		2007
#define SHM_KEY		2007 //ключи менять смысла нет

union semnum 
{
	int val;
	struct semid_ds * buf;
	unsigned short * array;
} sem_arg;


int main (void)
{
	int shm_id, sem_id;
	int * shm_buf; 
	int shm_size; 
	struct shmid_ds ds; 
	struct sembuf sb[1];
	unsigned short sem_vals[1]; 
	
	shm_id = shmget (SHM_KEY, SHMEM_SIZE,
			IPC_CREAT | 0600); //создаем сегмент, даем все права только владельцу

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
	sem_vals[0] = 1; //значением делаем 1
	sem_arg.array = sem_vals;

	if (semctl (sem_id, 0, SETALL, sem_arg) == -1)
	{
		fprintf (stderr, "semctl() error\n");
		return 1;
	}

    int num = 0;
    printf("Enter the number to count down from: ");
    scanf("%d", &num);
 
	shm_buf = (int *) shmat (shm_id, NULL, 0); //подключаем сегмент к адресному пространству процесса
	if (shm_buf == (int *) -1)
	{
		fprintf (stderr, "shmat() error\n");
		return 1;
	}

    *shm_buf = num;

    sb[0].sem_num = 0;
    sb[0].sem_flg = SEM_UNDO; 

	while((*shm_buf) > 0) {  //используем семафор для очередности вычитания
        sb[0].sem_op = -1; 
	    semop (sem_id, sb, 1); 
        printf("<1> current value: %d\n", *shm_buf);
        (*shm_buf)--;
        sleep(1);
    }

	semctl (sem_id, 0, IPC_RMID, sem_arg);
	shmdt (shm_buf); 
	shmctl (shm_id, IPC_RMID, NULL); 

	return 0;
}