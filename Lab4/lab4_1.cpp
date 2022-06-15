#include <iostream>
#include <unistd.h>
#include <locale.h>
using namespace std;


int main(int argc, char const *envp[])
{
	//setlocale(LC_ALL,"Russian");
	printf("Программа 1 запущена\n");
	printf("id дочернего процесса: %d\n", getpid());
	printf("id родительского процесса: %d\n", getppid());
	printf("Аргументы массива: \n");
	for (int i = 1; i < 8; i++)
	{
		printf("%d : %d\n", i, i);
		sleep(1);
	}
	printf("Окружение: \n");
	for (int j=0;j<3;j++)
	{
		printf("%s\n",envp[j]);
		sleep(1);
	}
	printf("Программа 1 завершила работу\n");
	return 0 ;
}
