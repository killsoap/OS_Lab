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
	sem_unlink("/semaphore1");
	sem_unlink("/semaphore2");
  	munmap(NULL, sizeof(buffer));
    close(shm);
    shm_unlink("/prog");
    
	exit(0);
}



int main()
{
    printf("программа 1 начала работу\n");

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
       perror("open sem1");
    }

    sem2 = sem_open("/semaphore2", O_CREAT, 0644, 0);
    if (sem2 == SEM_FAILED) 
    {
       perror("open sem2");
    }

    if (fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK) == -1) 
    { 
       perror("fcntl");
    }
    signal(SIGINT, sig_handler);
    
    while(true)
    {
        int sock, length;
        struct sockaddr_in name;
        
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) 
        {
            perror("socket");
        }

        name.sin_family = AF_INET;
        name.sin_addr.s_addr = INADDR_ANY;
        name.sin_port = 0;
        if (bind(sock,(struct sockaddr *)&name, sizeof name) == -1) 
        {
            perror("bind");
        }

        length = sizeof(name);

        if (getsockname(sock, (struct sockaddr *) &name, (socklen_t *) &length) == -1)
        {
            perror("getsockname");
        }

        uint16_t buffer1 = ntohs(name.sin_port);
        *buffer = {0};
        *buffer = buffer1;

        memcpy(v, buffer, sizeof(buffer));
        printf("%d\n", *buffer);

        sem_post(sem1);

        printf("Ожидание сефамора\n");
        
        sem_wait(sem2);
        sleep(1);
        int a = getchar();
        if (a != -1 ) 
        {
            break;
        }
    
    }
    sem_close(sem1);
    sem_close(sem2);
	sem_unlink("/semaphore1");
	sem_unlink("/semaphore2");
  	munmap(NULL, sizeof(buffer));
    close(shm);
    shm_unlink("/prog");

    printf("программа 1 завершила работу\n");

    return 0;
}
