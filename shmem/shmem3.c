/* Проверить совместную работу с 4.
   Написать комментарии, В ТОМ ЧИСЛЕ К ПАРАМЕТРАМ!*/
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>

#define SHMEM_SIZE	4096 //размер разделяемой памяти
#define SH_MESSAGE	"Poglad Kota!\n" //сообщение которое мы туда запишем

int main (void)
{
  int shm_id; // id сегмента разделяемой памяти
  char * shm_buf; // указатель на него
  int shm_size; // его размер
  struct shmid_ds ds; // структурка с информацией про сегмент
  
  shm_id = shmget (IPC_PRIVATE,
                   SHMEM_SIZE,
  		           IPC_CREAT | IPC_EXCL | 0600); // создаем сегмент
                   // IPC_PRIVATE - каждый раз создаем новый сегмент размера SHMEM_SIZE
                   // если сегмент существует, считаем это ошибкой. Возвращаем id сегмента
  
    // если не получилось создать сешмент, id будет -1
  if (shm_id == -1) 
  {
    fprintf (stderr, "shmget() error\n");
    return 1;
  }
  
  shm_buf = (char *) shmat (shm_id,
                            NULL,
                            0);  //добавляем сегмент к адресному пространству процесса, было две ошибки
  if (shm_buf == (char* )-1)
  {
    fprintf (stderr, "shmat() error\n");
  	return 1;
  }
  
  shmctl (shm_id,
          IPC_STAT,
          &ds);  // пишем в созданную структурку информацию о сегменте
  
  shm_size = ds.shm_segsz;
  if (shm_size < strlen (SH_MESSAGE))  // проверяем что строка запишется полностью
  {
  	fprintf (stderr, "error: segsize=%d\n", shm_size);
  	return 1;
  }
  
  strcpy (shm_buf,
          SH_MESSAGE); // копируем строчку в сегмент
  
  printf ("ID: %d\n", shm_id);
  printf ("Press <Enter> to exit...");	
  fgetc (stdin);
  
  shmdt (shm_buf);
  shmctl(shm_id,
         IPC_RMID,
         NULL); //завершаем
  
  return 0;
}