#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

using namespace std;

#define MAX_SIZE 1024

int sockfd;
struct sockaddr_in serverAddr;
socklen_t serverAddrlen;
void* createRequset (void* arg)
{
    int* farg = (int*)arg;
    char buffer[MAX_SIZE];
    int re_value;
    int count = 1;
    while(*farg==0)
    {
        re_value = sprintf(buffer,"%d", count);
        if(re_value < 0)
        {
            printf("Ошибка\n");
            pthread_exit(NULL);
        }

        printf("Отправка запроса %d\n",count);
        fflush(stdout);
        re_value = sendto(sockfd, buffer, MAX_SIZE, 0, (struct sockaddr*)&serverAddr, serverAddrlen);
        if (re_value==-1)
        {
            perror("sendto");
            pthread_exit(NULL);
        }
        else
            printf("Отправка выполнена\n");

        sleep(1);
        count ++;
    }
    pthread_exit(NULL);
}

void* receive (void* arg)
{
    int* farg = (int*)arg;
    char buffer[MAX_SIZE];
    int re_value;
    while(*farg==0)
    {
        memset(buffer, 0, MAX_SIZE);

        re_value = recvfrom(sockfd, buffer, MAX_SIZE, 0, (struct sockaddr*)&serverAddr, &serverAddrlen);
        if (re_value==-1)
        {
            perror("recvfrom");
            if (errno == EAGAIN)
            {
                sleep(1);
                continue;
            }
            pthread_exit(NULL);
        }

        printf("Принято: %s\n",buffer);
        fflush(stdout);
        sleep(1);
    }
    pthread_exit(NULL);
}

int main ()
{
    printf("Программа начала работу\n");
    pthread_t thread_request, thread_receive;
    int fReq, fRec;
    fReq = fRec = 0;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd==-1)
    {
        perror("socket");
        return 1;
    }
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    int optval = 1;
    int re_value = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if(re_value==-1)
    {
        perror("setsockopt");
        return 1;
    }

    struct sockaddr_in myAddr;
    memset((char *) &myAddr, 0, sizeof(myAddr));
    myAddr.sin_family =         AF_INET;
    myAddr.sin_port =		htons(7000);
    myAddr.sin_addr.s_addr =	inet_addr("127.0.0.1");

    re_value = bind(sockfd, (struct sockaddr*)&myAddr, sizeof(myAddr));

    memset((char *) &serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family =		AF_INET;
    serverAddr.sin_port =		htons(8000);
    serverAddr.sin_addr.s_addr =	inet_addr("127.0.0.1");
    serverAddrlen = sizeof(serverAddr);

    re_value = pthread_create(&thread_request, NULL, createRequset, &fReq);
    if (re_value!=0)
    {
        cout << strerror(re_value);
        return 1;
    }

    re_value = pthread_create(&thread_receive, NULL, receive, &fRec);
    if (re_value!=0)
    {
        cout << strerror(re_value);
        return 1;
    }

    getchar();
    printf("Кнопка нажата\n");
    fRec = fReq = 1;

    re_value = pthread_join(thread_request, NULL);
    if (re_value!=0)
    {
        cout << strerror(re_value);
        return 1;
    }

    re_value = pthread_join(thread_receive, NULL);
    if (re_value!=0)
    {
        cout << strerror(re_value);
        return 1;
    }

    re_value = close(sockfd);
    if (re_value==-1)
    {
        perror("close");
        return 1;
    }

    printf("Программа завершила работу\n");
    return 0;
}
