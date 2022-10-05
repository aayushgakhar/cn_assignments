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

typedef struct thread_arg {
    int socket_fd;
}thread_arg;

void *threadFunc(void *arg)
{
    // int new_socket = *((int *)arg);
    // char buffer[1024] = {0};
    // int valread;
    // while (1)
    // {
    //     valread = read(new_socket, buffer, 1024);
    //     printf("message received: %s", buffer);
    // }
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
    char server_messg[10240] = { 0 };
    char client_messg[10240] = { 0 };
    int i=0;
    while(1){
        printf("\n--------------------\n");
        
        bzero(client_messg,10240);
        sprintf(client_messg,"%d",i+1);
        printf("message sent: %s\n",client_messg);
        send(socket_fd, client_messg, strlen(client_messg), 0);
        
        // server_messg[0] = '\0';
        bzero(server_messg,10240);
        valread = recv( socket_fd , server_messg, 10240,0);
        printf("response recieved: %s\n",server_messg);
        i+=1;
        if(i>=20)
            break;
    }
    close(socket_fd);

    return 0;
}

int main(int argc, char const *argv[])
{

    int n = 10;
    pthread_t ptid[n];
    for (int i = 0; i < n; i++)
    {
        int status;
        status = pthread_create(&ptid[i], NULL, threadFunc, NULL);
    }
    sleep(1);
    for (int i = 0; i < n; i++)
    {
        int ret;
        pthread_join(ptid[i], (void**)&ret);
        printf("%d\n", ret);
    }
}