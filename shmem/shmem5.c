/* Эта программа позволяет обмениваться сообщениями между терминалами.
  Произвести изменения. 
  1 расширить работу на N терминалов, добавив имя пользователя в параметры программы
  и имя получателя при вводе строки.
  2 Отметить какие моменты сильно влияют на надёжность работы программы*/
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#define FILE_NAME "message.txt"
#define TEXT_SIZE 256

struct msg_t {
	int id;
	int sender;
	int reciever;
	int delivered;
	char text[TEXT_SIZE];
};

//Блокируем файл перед записью и чтением чтобы не было УБ из-за неопределенного порядка действий
void LockFile(int fd) { 
	struct flock lock;

	lock.l_type = F_WRLCK; //блокируем запись в файл
	lock.l_whence = SEEK_SET; 
	lock.l_start = 0; //блокируем с нулевого байта файла
	lock.l_len = 0; //и до конца

	fcntl(fd, F_SETLKW, &lock);
}

void UnlockFile(int fd) {
	struct flock lock;

	lock.l_type = F_UNLCK; //снимаем блокировку с файла так же как и ставили ее
	lock.l_whence = SEEK_SET; 
	lock.l_start = 0; 
	lock.l_len = 0; 

	fcntl(fd, F_SETLKW, &lock);
}

void send_msg(int sender, int reciever, char* text) {
	if (!text) return; //если нечего записывать, сразу возвращаемся

	int fd = open(FILE_NAME, O_RDWR | O_CREAT, 0666);

	if (fd < 0) { //и если файл открыть не получается - тоже
		printf("Can't open file\n");
		return;
	}

	LockFile(fd);

	struct msg_t msg; //что хотим отправить
	struct msg_t crnt; //что уже есть в файлике

	int  max_id = 0;
	lseek(fd, 0, SEEK_SET); //идем в начало файла

	while (read(fd, &crnt, sizeof(crnt)) == sizeof(crnt)) {
		if (crnt.id > max_id) max_id = crnt.id; //ищем сообщение с наибольшим id
	}

	msg.id = max_id + 1;
	msg.sender = sender;
	msg.reciever = reciever;
	msg.delivered = 0;

	strncpy(msg.text, text, TEXT_SIZE - 1);
	msg.text[TEXT_SIZE - 1] = '\0';

	lseek(fd, 0, SEEK_END); //идем в конец файлика
	write(fd, &msg, sizeof(msg)); //и пишем сообщение

	UnlockFile(fd);
}


void RecieveMessage(int user_id) {
	int fd = open(FILE_NAME, O_RDWR | O_CREAT, 0666);
	if (fd < 0) return; //если не получилось открыть файл, то и сообщений быть не может

	LockFile(fd);

	struct msg_t msg;
	off_t pos = 0; //позиция в файле

	while(pread(fd, &msg, sizeof(msg), pos) == sizeof(msg)) {
		if ((msg.reciever == user_id) && (msg.delivered == 0)) {
			printf("Message from user %d:\n", msg.sender);
			printf("%s\n\n", msg.text);

			msg.delivered = 1; //прочитали сообщение, вывели и можно пометить как прочитанное
			pwrite(fd, &msg, sizeof(msg), pos); //перезаписываем
		}

		pos += sizeof(msg);
	}

	UnlockFile(fd);
	close(fd);
}


int main(int argc, char* argv[])
{
	if (argc < 2) {
		printf("Not enough arguments\n");
		return 1;
	}

	int user_id = atoi(argv[1]);
	printf("User %d currently in charge\n", user_id);

	pid_t child_pid;

	printf(
		"--------------------\n"
		"Terminals-Chat 0.1\n"
		"--------------------\n"
		"To send message just type in the reciever, then type the message and press Enter.\n"
		"--------------------\n" );

	pid_t childpid;
	if ((childpid = fork()) == -1)
	{
		perror("fork error");
		return 1;
	}
	
	if(childpid == 0)
	{
		// child process: just check for incoming messages

		for(;;)
		{
			RecieveMessage(user_id);
			sleep(1);
		}

	} else
	{
		// parent process: send messages
		
		int reciever;
		char text[TEXT_SIZE];

		for(;;)
		{
			printf("Reciever: ");
			if (scanf("%d", &reciever) != -1) break;

			getchar();
			printf("Message: ");

			if (fgets(text, sizeof(text), stdin) == NULL) break;

            text[strcspn(text, "\n")] = '\0';

            if (strcmp(text, "/quit") == 0) break;

            SendMessage(user_id, reciever, text);
        }

        kill(childpid, SIGTERM);
        wait(NULL);
    }

    return 0;
}