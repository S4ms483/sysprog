/* Программа, иллюстрирующая использование системных вызовов open(), read() и close() для чтения информации из файла */

#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
int fd;
ssize_t size;
char string[60];

/* Попытаемся открыть файл с именем в первом параметре выззова только
для операций чтения */

if((fd = open(argv[1], O_RDONLY)) < 0)
{
  /* Если файл открыть не удалось, печатаем об этом сообщение и прекращаем работу */
  printf("Can\'t open file\n");
  exit(-1);
}

/* Читаем фаил пока не кончится и печатаем */
 size = read(fd, string, 59);
 string[59] = '\0';
 printf("%s\n", string); /* Печатаем прочитанное*/

/*  Записываем файл под новым именем */
  int cpy_fd = open("copy.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

  if (cpy_fd < 0) {
    fprintf(stderr, "Can't copy given file to another one\n");
    exit(-1);
  }

  if (write(cpy_fd, string, size) != size){
    fprintf(stderr, "Couldn't write string to the new file completely\n");
  }
/* Закрываем файл */
if(close(fd) < 0)
{
  printf("Can\'t close file\n");
}
/*  Открываем файл в редакторе */
execlp("nano", "nano", "copy.txt", (char *)NULL);
perror("Can't open editor");

return 0;
} 