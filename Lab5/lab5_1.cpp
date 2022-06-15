#include <stdio.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

sem_t* some_sem;
const char* sem_name = "/some_sem";

int main()
{
	printf("Программа 1 запущена\n");
	
	 
	FILE *fp;
	
	fp = fopen("text.txt", "a"); //открытие файла
	
	some_sem = sem_open(sem_name,O_CREAT, S_IRWXU, 1); //открытие семафора

	
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    int flag = 1;
	
	while (flag != 10){
		sem_wait(some_sem);
		for(size_t i = 0; i<10;++i){
			fprintf(fp, "%d", 1);
			fflush(fp);
			printf("%d",1);
			fflush(STDIN_FILENO);
			sleep(1);
		}
			printf("\n");
			fprintf(fp," ");
			sem_post(some_sem);
			sleep(1);
			flag = getchar();

			
		}
	
	
	
	fclose(fp);
    sem_close(some_sem);
    sem_unlink(sem_name);

	
	
	printf("Программа 1 завершила работу\n");
	return 0 ;
}
