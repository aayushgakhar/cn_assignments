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
char *get_ip(struct sockaddr *sa)
{
    static char s[INET6_ADDRSTRLEN];
    switch (sa->sa_family)
    {
    case AF_INET:
        inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr), s, sizeof(s));
        break;
    case AF_INET6:
        inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr), s, sizeof(s));
        break;
    default:
        strcpy(s, "Unknown AF");
        break;
    }
    return s;
}

int get_port(struct sockaddr *sa)
{
    switch (sa->sa_family)
    {
    case AF_INET:
        return ntohs(((struct sockaddr_in *)sa)->sin_port);
    case AF_INET6:
        return ntohs(((struct sockaddr_in6 *)sa)->sin6_port);
    default:
        return -1;
    }
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

        printf("\n%s\n>>request: %s, Sending response: %s\n",SEP, request, server_messg);

        fprintf(fp, "\n%s\n>>request: %s, response: %s, IP: %s, Port: %d\n\n", SEP, request, server_messg, ip, port);
        
        send(sock_fd, server_messg, strlen(server_messg), 0);
    }
    close(sock_fd);
    fflush(fp);
}

int main()
{
    int server_fd, valread;
    struct sockaddr_in server_addr, client_addr;
    int opt = 1;
    int addrlen;

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
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    // open file
    FILE *fp;
    fp = fopen("output/2b.txt", "w");
    if (fp == NULL)
    {
        perror("fopen");
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
        
        if (fork() == 0)
        {
            // get ip and port
            char *ip = get_ip((struct sockaddr *)&client_addr);
            int port = get_port((struct sockaddr *)&client_addr);
            // write ip and port to file
            fprintf(fp, "New connection...\nIP: %s, Port: %d\n", ip, port);

            forkFunc(sock_fd, fp, ip, port);
            break;
        }
        else
        {
            continue;
        }
    }
    close(server_fd);
    fflush(fp);
    fclose(fp);

    return 0;
}