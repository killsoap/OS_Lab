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
    mes.type = 1;
    int sock,ss,res;
    size_t n;
    socklen_t szi = sizeof(int);
    sock=socket(AF_INET,SOCK_STREAM,0);	
    while (flag == 0)
    {
        getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*) &ss, &szi);
        n = sprintf (mes.buff, "%d", ss);
        printf("%d\n",ss);
        fflush(stdout);
        if ((res = msgsnd(msgid, &mes, n, IPC_NOWAIT)) == -1)
        {
		printf("Передача сообщения не выполнена %s\n", strerror(errno));
	 	sleep(1);  
        }
        
        sleep(1);
    }
    close(sock);
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
