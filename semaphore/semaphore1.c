#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h> //т.к. есть exit()

int main(int argc, char *argv[], char *envp[])
{
  int   semid; //id набора семафоров
  char pathname[]="semaphore1.c"; //файл который генерирует ключ
  key_t key; //ipc-ключ
  struct sembuf mybuf; //операциянад семафором
  
  key = ftok(pathname, 0); //создаем ключ
  
  if((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) //создаем набор семафоров
  {
    printf("Can\'t create semaphore set\n");
    exit(-1);
  }
  
  mybuf.sem_num = 0; //т.к. семафор в наборе один - его номер - 0
  mybuf.sem_op  = -1; //уменьшаем его значение на 1
  mybuf.sem_flg = 0; //как бы отсутствие каких-то особенностей поведения
  
  if(semop(semid, &mybuf, 1) < 0)
  {
    printf("Can\'t wait for condition\n"); //если не можем уменьшить значение семафора - ждем
    exit(-1);
  }  
    
  printf("The condition is present\n");
  return 0;
}