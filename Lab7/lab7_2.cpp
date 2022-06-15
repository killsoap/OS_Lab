#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/msg.h>

int flag=0;
int msgid;

typedef struct 
{
	long type;
	char buff[5];
}TMessage;

void *proc(void *args)
{
	printf("Поток начал работу\n");
	fflush(stdout);
	TMessage mes;
	ssize_t res;
	while (flag == 0)
	{
		mes.type = 1;
		memset(mes.buff,0,sizeof (mes.buff));
		res = msgrcv(msgid, &mes, sizeof (mes.buff), mes.type, IPC_NOWAIT);
		if (res!=5)
		{
			printf("Буффер пуст\n");
			sleep(1);
		}
		else
		{
			printf("Принято: %s\n",mes.buff);
			sleep(1);
		}
	}
	void* result = (void*) 3;
	pthread_exit(result);
}

int main()
{
	pthread_t id;
	key_t key;
	struct msqid_ds msqid_ds, buf;
	key = ftok("lab7_2.sh", 'a');
	msgid = msgget(key,0);
	if (msgid < 0) 
	{
		msgid = msgget(key, IPC_CREAT | 0644);
	}
	
	msgctl(msgid, IPC_STAT, &buf);
	
	printf("Размер очереди %ld\n",buf.msg_qbytes);
	
	buf.msg_qbytes +=16384;
	
	msgctl(msgid, IPC_SET, &buf);
	
	msgctl(msgid, IPC_STAT, &buf);
	
	printf("Размер очереди %ld\n",buf.msg_qbytes);
	
	pthread_create(&id, NULL, proc,NULL);
	
	printf("Ожидание нажатия клавиши\n");
	
	getchar();
	
	printf("Клавиша нажата\n");
	
	flag = 1;
	
	pthread_join(id, NULL);
	
	close(msgid);
	
	msgctl(msgid, IPC_RMID, NULL);
	
	printf("Программа завершила работу\n");
	
	return 0;
}
