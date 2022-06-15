#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>        
#include <sys/stat.h>  
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>



#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>

using namespace std;

sem_t *sem1;
sem_t *sem2;

int shm;
unsigned short buffer[1024];
void* v;



void sig_handler (int signo)
{
    sem_close(sem1);
    sem_close(sem2);
	sem_unlink("/semapphore1");
	sem_unlink("/semaphore2");
  	munmap(NULL, sizeof(buffer));
    close(shm);
    shm_unlink("/prog");
    
	exit(0);
}



int main()
{
    printf("программа 2 начала работу\n");

    shm = shm_open("/prog", O_CREAT|O_RDWR, 0644);
    if (shm == -1)
    {
        perror("shm_open");
    }

    if(ftruncate(shm, sizeof(buffer)) == -1)
    {
        perror("ftruncate");
    }
    
    v = mmap(NULL, sizeof(buffer), PROT_READ|PROT_WRITE, MAP_SHARED, shm, 0);

    sem1 = sem_open("/semaphore1", O_CREAT, 0644, 0);
    if (sem1 == SEM_FAILED) 
    {
       perror("sem_open1");
    }

    sem2 = sem_open("/semaphore2", O_CREAT, 0644, 0);
    if (sem2 == SEM_FAILED) 
    {
       perror("sem_open2");
    }

    if (fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK) == -1) 
    { 
       perror("fcntl");
    }
    signal(SIGINT, sig_handler);
    
    while(true)
    {   

        printf("Ожидание симафора\n");

        sem_wait(sem1);

        memcpy(buffer, v, sizeof(buffer));
        printf("%d\n", *buffer);

        sem_post(sem2);
        int a = getchar();
        if (a != -1 ) 
        {
            break;
        }

    }
    
    
    sem_close(sem1);
    sem_close(sem2);
	sem_unlink("/semapphore1");
	sem_unlink("/semaphore2");
  	munmap(NULL, sizeof(buffer));
    close(shm);
    shm_unlink("/prog");

    printf("программа 2 завершила работу\n");

    return 0;
}
