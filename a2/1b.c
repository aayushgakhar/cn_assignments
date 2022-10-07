#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>

#define PORT 8080
#define MESG_SIZE 2000



unsigned long long int fac(int n)
{
    unsigned long long int p = 1;
    while (n > 0)
    {
        p *= n;
        n--;
    }
    return p;
}

void *threadFunc(void *arg)
{
    int *id = (int *)arg;
    int socket_fd = 0, valread;
    struct sockaddr_in server_addr;
    
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return 0;
    }
    printf("Socket created%d\n", *id);
 
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr)<= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return 0;
    }
 
    if (connect(socket_fd, (struct sockaddr*)&server_addr,sizeof(server_addr)) < 0) {
        printf("\nConnection Failed \n");
        return 0;
    }
    printf("Connected%d\n", *id);
    char server_messg[MESG_SIZE] = { 0 };
    char client_messg[MESG_SIZE] = { 0 };
    // int i = rand()%20;
    int  i = *id;
    // while(i<20){
    memset(client_messg, 0, MESG_SIZE);
    sprintf(client_messg, "%d", i + 1);

    send(socket_fd, client_messg, strlen(client_messg), 0);
    printf("thread: %d message sent: %s\n", *id, client_messg);
    memset(server_messg, 0, MESG_SIZE);
    valread = recv(socket_fd, server_messg, MESG_SIZE, 0);
    if (valread == 0){
        printf("Connection closed\n");
    }
    printf("\n--------------------\n");
    printf("thread: %d message sent: %s\n", *id, client_messg);
    printf("response recieved: %s", server_messg);
    char *p;
    int ret = 1;
    if (strtoull(server_messg, &p, 10) == fac(i + 1))
    {
        // print tick symbol
        printf("\033[0;32m");
        printf(" \u2713\n");
        printf("\033[0m");
        ret = 0;
    }
    // i += 1;
    // }
    close(socket_fd);
    pthread_exit((void *)&ret);
}

int main(int argc, char const *argv[])
{

    int n = 10;
    pthread_t ptid[n];
    int a[n];
    for (int i = 0; i < n; i++)
    {
        int status;
        a[i] = i;

        status = pthread_create(&ptid[i], NULL, threadFunc, (void *)&a[i]);
    }
    int w = 0;
    for (int i = 0; i < n; i++)
    {
        int* ret;
        pthread_join(ptid[i], (void**)&ret);
        w += *ret;
    }
    printf("Total wrong: %d\n", w);
}