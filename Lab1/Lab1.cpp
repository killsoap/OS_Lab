#include <iostream>
#include <unistd.h>
#include <signal.h> 

	pthread_t id1;
	pthread_t id2;

typedef struct 
{
	int flag;
	char sym;
}targs;

void * proc1(void *arg){	
	printf("Поток 1 начал работу\n");
	targs *args = (targs*) arg;
	sigset_t *set;
	int ret, sig;
	while (args->flag == 0) {
		
		ret = sigwait(set, &sig);
		
		putchar(sig);
		fflush(stdout);
		sleep(1);
}
	printf("Поток 1 закончил работу\n");
}

void * proc2(void *arg){
	printf("Поток 2 начал работу\n");
	targs *args = (targs*) arg;
	while (args->flag == 0) {
		
		pthread_kill (id1, SIGUSR1);
		
		
		//putchar(args->sym);
		//fflush(stdout);
		sleep(1);
	
}
	printf("Поток 2 закончил работу\n");
}

int main(){
	printf("Программа начала работу\n");
	struct timespec ts;
	targs arg1, arg2;
	arg1.flag	= 0;		
	arg1.sym	= '1';
	arg2.flag	= 0;	
	arg2.sym	= '2';
	
	clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 5;

	
	pthread_create(&id1,NULL, proc1, &arg1); // Создание потока, выполняющего функцию 
	sleep (2);
	pthread_create(&id2,NULL,proc2,&arg2);
	printf("Программа ждёт нажатия клавиши\n");
	getchar(); //Ожидание нажтия кнопки
	printf("Клавиша нажата\n");
	//arg1.flag = 0;
	//arg2.flag = 0;
	pthread_timedjoin_np(id1,NULL,&ts); //приостановка основного потока до завершения выбранного
	pthread_timedjoin_np(id2,NULL,&ts);
	printf("Программа завершила работу\n");
}
