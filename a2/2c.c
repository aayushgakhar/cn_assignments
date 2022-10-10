#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>

#define PORT 8080
#define MESG_SIZE 2000
#define SEP "--------------------"

typedef struct thread_arg_t
{
    int socket_fd;
    FILE *fp;
    char *ip;
    int port;
} thread_arg_t;

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

void *threadFunc(void *arg)
{
    thread_arg_t *p_arg = (thread_arg_t *)arg;

    int sock_fd = p_arg->socket_fd;
    FILE *fp = p_arg->fp;
    char *ip = p_arg->ip;
    int port = p_arg->port;
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

        printf("\n%s\n>>request: %s, Sending response: %s\n", SEP, request, server_messg);

        fprintf(fp, "\n%s\n>>request: %s, response: %s, IP: %s, Port: %d\n", SEP, request, server_messg, ip, port);

        send(sock_fd, server_messg, strlen(server_messg), 0);
    }
    close(sock_fd);
    fflush(fp);
    
}

int main()
{
    int server_fd, valread;
    struct sockaddr_in server_addr, client_addr;
    // get pid
    pid_t pid = getpid();
    printf("pid: %d\n", pid);
    int opt = 1;
    int addrlen;
    thread_arg_t* p_arg;

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
    int i = 0;
    
    if (listen(server_fd, 32) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    // open file
    FILE *fp;
    fp = fopen("output/2c.txt", "w");
    if (fp == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
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
        // write ip and port to file
        fprintf(fp, "\nNew connection...\nIP: %s, Port: %d\n\n", ip, port);

        p_arg = (thread_arg_t *)malloc(sizeof *p_arg);

        p_arg->socket_fd = sock_fd;
        p_arg->fp = fp;
        p_arg->ip = ip;
        p_arg->port = port;
        int status;
        status = pthread_create(&ptid[i], NULL, threadFunc, (void *)p_arg);
        i++;
    }
    close(server_fd);
    fflush(fp);
    fclose(fp);
    return 0;
}