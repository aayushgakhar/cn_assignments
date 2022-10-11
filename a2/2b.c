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
#include "util.h"


#define SEP "--------------------"

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

// function returns ip from address
char *get_ip(struct sockaddr_in *sin)
{
    static char s[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &((sin)->sin_addr), s, sizeof(s));
    return s;
}

int get_port(struct sockaddr_in *sin)
{
    return ntohs(sin->sin_port);
}

void forkFunc(int sock_fd, FILE* fp, char* ip, int port)
{
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

        long long int response = fac(atoi(request));
        memset(server_messg, 0, MESG_SIZE);
        sprintf(server_messg, "%llu", response);
        if(PRINT){
            printf("\n%s\n>>request: %s, Sending response: %s\n",SEP, request, server_messg);
        }
        if(FILE_PRINT){
            fprintf(fp, "request: %s, response: %s, IP: %s, Port: %d\n", request, server_messg, ip, port);
            fflush(fp);
        }

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
    pid_t pid = getpid();
    printf("pid: %d\n", pid);

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
    if (listen(server_fd, 32) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    // open file
    FILE *fp;
    if(FILE_PRINT){
        fp = fopen("output/2b.txt", "w");
        if (fp == NULL)
        {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
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
        // get ip and port
        char *ip = get_ip(&client_addr);
        int port = get_port(&client_addr);

        if (fork() == 0)
        {
            forkFunc(sock_fd, fp, ip, port);
            break;
        }
        else
        {
            continue;
        }
    }
    close(server_fd);

    return 0;
}