#include <iostream>
#include <unistd.h>
#include <signal.h> 

pthread_mutex_t mutex;
clockid_t clk_id = CLOCK_REALTIME;
int flag1 = 0, flag2 = 0;
struct timespec tp;

void * proc1(void *arg){	
	printf("Поток 1 начал работу\n");
	int k;
	while (flag1 != 1) {
		do {clock_gettime(clk_id,&tp); tp.tv_sec += 1;
			k = pthread_mutex_timedlock(&mutex, &tp);}while (k != 0);
		for(size_t i=0; i<5; i++){
		putchar('1');
		fflush(stdout);
		sleep(1);
	}
		pthread_mutex_unlock(&mutex);
		sleep(1);
}
	printf("Поток 1 закончил работу\n");
}

void * proc2(void *arg){	
	printf("Поток 2 начал работу\n");
	int k;
	while (flag2 != 1) {
		do {clock_gettime(clk_id,&tp);
		tp.tv_sec += 1;
			k = pthread_mutex_timedlock(&mutex, &tp);}while (k != 0);
		for(size_t i=0; i<5; i++){
		putchar('2');
		fflush(stdout);
		sleep(1);
	}
		pthread_mutex_unlock(&mutex);
		sleep(1);
}
	printf("Поток 2 закончил работу\n");
}

int main(){
	printf("Программа начала работу\n");
	pthread_t id1;
	pthread_t id2;

	
	pthread_mutex_init(&mutex, NULL);
	clock_gettime(clk_id,&tp);
	
	tp.tv_sec += 1;
	
	
	pthread_create(&id1,NULL, proc1, NULL); // Создание потока, выполняющего функцию 
	pthread_create(&id2,NULL,proc2,NULL);
	printf("Программа ждёт нажатия клавиши\n");
	getchar(); //Ожидание нажтия кнопки
	printf("Клавиша нажата\n");
	flag1 = 1;
	flag2 = 1;
	pthread_join(id1,NULL); //приостановка основного потока до завершения выбранного
	pthread_join(id2,NULL);
	
	pthread_mutex_destroy(&mutex);
	
	printf("Программа завершила работу\n");
}
