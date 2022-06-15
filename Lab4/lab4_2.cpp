#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <cstring>
#include <locale.h>
#include <sys/wait.h>
#include <stdlib.h>
using namespace std;

int main()
{
	printf("Программа 2 запущена\n");
	pid_t pid = fork();
	if (pid == 0)
	{
		printf("Дочерний процесс запущен\n");
		printf("id дочернего процесса: %d\n", getpid());
		printf("id родительского процесса: %d\n",getppid());
		char* const envp[]={"ONE","TWO","THREE",NULL};
		execle("lab4_1", "1", "2", "3", "4", "5", "6", "7", NULL,envp);
		printf("Дочерний процесс %d завершён\n",getpid());
	}
	else if (pid > 0)
	{
		printf("Родительский процесс запущен\n");
		printf("id родительского процесса: %d\n",getpid());
		printf("id дочернего процесса: %d\n",pid);
		pid_t check = 0;
		int status;
		while (check <= 0)
		{
			printf("Дочерний процесс не завершен\n");
			check = waitpid(pid, &status, WNOHANG);
			sleep(1);
		}
		printf("Дочерний процесс завершен с кодом %d\n",WEXITSTATUS(status));
	}
	else
	{
		perror("fork "); 
	}
	printf("Программа 2 завершила работу");
	return 0;
}
