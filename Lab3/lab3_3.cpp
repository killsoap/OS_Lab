#include <iostream>
#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
#include<mutex>
#include <cstring>
#include <time.h>
 #include <stdlib.h>
 #include<fcntl.h>


int filedes[2];
int flag1 = 0, flag2 = 0;

void * proc1(void *arg){	
	printf("Поток 1 начал работу\n");
	while (flag1 != 1) {
		long host;
		host = gethostid();
		write(filedes[1], &host, sizeof(host));
		printf("Запись %ld\n", host);
		sleep(1);
	}
	printf("Поток 1 закончил работу\n");
	pthread_exit((void*)1);
}

void * proc2(void *arg){	
	
	printf("Поток 2 начал работу\n");
	
	long host;
	
	while (flag2 != 1) {
	int check = read(filedes[0], &host, sizeof(host));
        if (check == -1)
        {
			char* error = strerror(errno);
			printf("Пустой буфер: %s\n", error);
			fflush(stdout);
            sleep(1);
        }
        else if (check == 0)
        {
			printf("Файл закрыт\n");
			fflush(stdout);
            sleep(1);
        }
        else
        {
		printf("Чтение: %ld\n", host);
		fflush(stdout);
		sleep(1);
        }
}
	printf("Поток 2 закончил работу\n");
	pthread_exit((void*)2);
}


int main()
{
	pipe(filedes);

  pthread_t id1, id2;

  int exitcode1=0;
  int exitcode2=0;

  fcntl(filedes[0], F_SETFL,flag1|O_NONBLOCK);
  fcntl(filedes[1], F_SETFL,flag2|O_NONBLOCK);
  
  pthread_create(&id1,NULL, proc1, NULL); // Создание потока, выполняющего функцию 
  pthread_create(&id2,NULL,proc2,NULL);

  getchar();

  flag1 = 1;
  flag2 = 1;

  pthread_join(id1, (void**)& exitcode1);
  pthread_join(id2, (void**)& exitcode2);

  printf("Поток 1 завершился с кодом: %d\n", exitcode1);
  printf("Поток 2 завершился с кодом: %d\n", exitcode2);

  close(filedes[0]);
  close(filedes[1]);

  return 0;
}
