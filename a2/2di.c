#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
#include <assert.h>
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

void set_nonblock(int socket)
{
    int flags;
    flags = fcntl(socket, F_GETFL, 0);
    assert(flags != -1);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in *)sa)->sin_addr);

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main()
{
    int server_fd, sock_fd, valread;
    struct sockaddr_in server_addr, client_addr;
    int opt = 1;
    int addrlen;
    int num_clients = 10;
    int sock_fds[num_clients];

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
    fd_set rdset;

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    // open file
    FILE *fp;
    fp = fopen("output/2di.txt", "w");
    if (fp == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    printf("Waiting for connection...");
    while (1)
    {
        FD_ZERO(&rdset);
        FD_SET(0, &rdset);

        addrlen = sizeof(client_addr);
        if ((sock_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // get ip and port
        char *ip = get_ip((struct sockaddr *)&client_addr);
        int port = get_port((struct sockaddr *)&client_addr);
        // write ip and port to file
        fprintf(fp, "New connection...\nIP: %s, Port: %d\n", ip, port);

        char client_messg[MESG_SIZE] = {0};
        char server_messg[MESG_SIZE] = {0};
        char *request, *str, *lID;
        int maxID = 0;
        while (1)
        {
            memset(client_messg, 0, MESG_SIZE);
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
        printf("Client disconnected\n");
        close(sock_fd);
        fflush(fp);
    }
    close(server_fd);
    fflush(fp);
    fclose(fp);
    return 0;
}