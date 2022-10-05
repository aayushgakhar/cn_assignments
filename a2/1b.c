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

void *threadFunc(void *arg)
{
    int *id = (int *)arg;
    int socket_fd = 0, valread;
    struct sockaddr_in server_addr;
    
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return 0;
    }
 
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
    char server_messg[MESG_SIZE] = { 0 };
    char client_messg[MESG_SIZE] = { 0 };
    int i=0;
    while(i<20){
        bzero(client_messg,MESG_SIZE);
        sprintf(client_messg,"%d",i+1);
        
        send(socket_fd, client_messg, strlen(client_messg), 0);
        
        // server_messg[0] = '\0';
        bzero(server_messg,MESG_SIZE);
        valread = recv( socket_fd , server_messg, MESG_SIZE,0);
        if(valread==0)break;
        printf("\n--------------------\n");
        printf("thread: %d message sent: %s\n", *id, client_messg);
        printf("response recieved: %s\n",server_messg);
        i+=1;
    }
    close(socket_fd);

    return 0;
}

int main(int argc, char const *argv[])
{

    int n = 10;
    pthread_t ptid[n];
    int a[10];
    for (int i = 0; i < n; i++)
    {
        int status;
        a[i] = i;

        status = pthread_create(&ptid[i], NULL, threadFunc, (void *)&a[i]);
    }
    sleep(1);
    for (int i = 0; i < n; i++)
    {
        int ret;
        pthread_join(ptid[i], (void**)&ret);
        printf("%d\n", ret);
    }
}