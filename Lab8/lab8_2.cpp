#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <vector>
#include <semaphore.h>


using namespace std;

#define MAX_SIZE 1024

int sockfd;
struct sockaddr_in clientAddr;
socklen_t clientAddrlen;
vector<string> listRequest;
sem_t sem;

void* createAnswer (void* arg)
{
    int* farg = (int*)arg;
    char buffer[MAX_SIZE];
    int re_value;
    while(*farg==0)
    {
        re_value = sem_wait(&sem);
        if (re_value!=0)
        {
            perror("sem_wait");
            pthread_exit(NULL);
        }
        if (!listRequest.empty())
        {
            string request = listRequest.back();
            listRequest.pop_back();
            re_value = sem_post(&sem);
            if (re_value!=0)
            {
                perror("sem_post");
                pthread_exit(NULL);
            }

            memset(buffer, 0, MAX_SIZE);
            int countRequest;
            re_value = sscanf(request.c_str(), "%d", &countRequest);

            bool optval;
            socklen_t optlen = sizeof(bool);
            re_value = getsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, &optlen);
            if(re_value==-1)
            {
                perror("getsockopt");
                pthread_exit(NULL);
            }

            re_value = snprintf(buffer, MAX_SIZE, "%d: SO_REUSEADDR is %s\n", countRequest, (optval==true)?"ON":"OFF");
            if(re_value < 0)
            {
                printf("Ошибка\n");
                pthread_exit(NULL);
            }

            re_value = sendto(sockfd, buffer, MAX_SIZE, 0, (struct sockaddr*)&clientAddr, clientAddrlen);
            if (re_value==-1)
            {
                perror("sendto");
                pthread_exit(NULL);
            }
            printf("Отправка %s\n",buffer);
            fflush(stdout);
        }
        else
        {
            re_value = sem_post(&sem);
            if (re_value!=0)
            {
                perror("sem_post");
                pthread_exit(NULL);
            }
            sleep(1);
        }
    }
    pthread_exit(NULL);
}

void* receive (void* arg)
{
    int* farg = (int*)arg;
    char buffer[MAX_SIZE];
    ssize_t re_value;
    while(*farg==0)
    {
        memset(buffer, 0, MAX_SIZE);
        re_value = recvfrom(sockfd, buffer, MAX_SIZE, 0, (struct sockaddr*)&clientAddr, &clientAddrlen);
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
        else
        {
            printf("Запрос %s получен\n",buffer);
            fflush(stdout);
        }

        re_value = sem_wait(&sem);
        if (re_value!=0)
        {
            perror("sem_wait");
            pthread_exit(NULL);
        }

        listRequest.push_back(string(buffer));

        re_value = sem_post(&sem);
        if (re_value!=0)
        {
            perror("sem_post");
            pthread_exit(NULL);
        }
    }
    pthread_exit(NULL);
}

int main ()
{
    printf("Программа начала работу\n");
    pthread_t thread_answer, thread_receive;
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
    myAddr.sin_family =		AF_INET;
    myAddr.sin_port =		htons(8000);
    myAddr.sin_addr.s_addr =	inet_addr("127.0.0.1");

    re_value = bind(sockfd, (struct sockaddr*)&myAddr, sizeof(myAddr));

    memset((char *) &clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family =		AF_INET;
    clientAddr.sin_port =		htons(7000);
    clientAddr.sin_addr.s_addr =	inet_addr("127.0.0.1");
    clientAddrlen = sizeof(clientAddr);

    re_value = sem_init(&sem, 0, 1);
    if(re_value!=0)
    {
        perror("sem_init");
        return 1;
    }

    re_value = pthread_create(&thread_receive, NULL, receive, &fRec);
    if (re_value!=0)
    {
        cout << strerror(re_value);
        return 1;
    }

    re_value = pthread_create(&thread_answer, NULL, createAnswer, &fReq);
    if (re_value!=0)
    {
        cout << strerror(re_value);
        return 1;
    }

    getchar();
    printf("Кнопка нажата\n");
    fRec = fReq = 1;

    re_value = pthread_join(thread_receive, NULL);
    if (re_value!=0)
    {
        cout << strerror(re_value);
        return 1;
    }

    re_value = pthread_join(thread_answer, NULL);
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
