#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
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

void threadFunc(int sock_fd)
{
    // thread_arg_t *p_arg = (thread_arg_t *)arg;
    // int sock_fd = p_arg->socket_fd;
    int valread;

    char client_messg[MESG_SIZE] = {0};
    char server_messg[MESG_SIZE] = {0};
    char *request, *str, *lID;
    int maxID = 0;
    while (1)
    {
        memset(client_messg, 0, MESG_SIZE * sizeof(client_messg[0]));
        valread = recv(sock_fd, client_messg, MESG_SIZE, 0);
        if (valread == 0)
            break;
        // request = strtok(client_messg, " ");
        request = client_messg;
        printf("\n--------------------\nrequest: %s\n", request);

        long long int response = fac(atoi(request));
        memset(server_messg, 0, MESG_SIZE);
        sprintf(server_messg, "%llu", response);
        printf("Sending response: %s\n", server_messg);
        send(sock_fd, server_messg, strlen(server_messg), 0);
    }
    close(sock_fd);
}

int main()
{
    int server_fd, valread;
    struct sockaddr_in server_addr, client_addr;
    int opt = 1;
    int addrlen;
    // thread_arg_t* p_arg;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    addrlen = sizeof(server_addr);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    pthread_t ptid[1000];
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    int i = 0;
    while (1)
    {
        addrlen = sizeof(client_addr);
        int sock_fd;
        if ((sock_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        if(fork()==0){
            threadFunc(sock_fd);
            break;
        }
        else
        {
            continue;
        }
        // p_arg = (thread_arg_t *)malloc(sizeof *p_arg);

        // p_arg->socket_fd = sock_fd;
        // int status;
        // status = pthread_create(&ptid[i], NULL, threadFunc, (void *)p_arg);
        // i++;
    }
    close(server_fd);
    return 0;
}