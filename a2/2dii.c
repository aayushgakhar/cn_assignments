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
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <poll.h>
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

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in *)sa)->sin_addr);

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main()
{
    
    int server_fd, valread;
    struct sockaddr_in server_addr, client_addr;
    int opt = 1;
    int addrlen;
    pid_t pid = getpid();
    printf("pid: %d\n", pid);
    
    int maxfd,status;
    int pfd_size = 11, pfd_count = 0;
    struct pollfd sock_fds[pfd_size];

    struct sockaddr_in addrs[pfd_size];
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // fcntl(server_fd, F_SETFL, O_NONBLOCK);
    
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
    sock_fds[0].fd = server_fd;
    sock_fds[0].events = POLLIN;
    pfd_count = 1;
    // open file
    FILE *fp;
    if(FILE_PRINT){
        fp = fopen("output/2dii.txt", "w");
        if (fp == NULL)
        {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
    }
    // fd_set rdset;
    addrlen = sizeof(client_addr);
    printf("Waiting for connection...");

    char client_messg[MESG_SIZE] = {0};
    char server_messg[MESG_SIZE] = {0};

    while (1)
    {
        int poll_status = poll(sock_fds, pfd_count, -1);
        if (poll_status < 0)
        {
            perror("poll");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < pfd_count; i++)
        {
            if (sock_fds[i].revents & POLLIN)
            {
                if (sock_fds[i].fd == server_fd)
                {
                    int new_socket = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
                    if (new_socket < 0)
                    {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }
                    if(PRINT){
                        printf("New connection , socket fd is %d , ip: %s , port : %d\n", new_socket, get_ip(&client_addr), get_port(&client_addr));
                    }
                    sock_fds[pfd_count].fd = new_socket;
                    sock_fds[pfd_count].events = POLLIN;
                    addrs[pfd_count] = client_addr;
                    pfd_count++;
                }
                else{
                    int sock_fd = sock_fds[i].fd;
                    memset(client_messg, 0, MESG_SIZE);
                    valread = recv(sock_fd, client_messg, MESG_SIZE,0);
                    if (valread < 0)
                    {
                        perror("read");
                        exit(EXIT_FAILURE);
                    }
                    if (valread == 0)
                    {
                        if(PRINT){
                            printf("Client disconnected, ip: %s , port : %d\n", get_ip(&addrs[i]), get_port(&addrs[i]));
                        }
                        close(sock_fd);
                        sock_fds[i].fd = sock_fds[pfd_count - 1].fd;
                        addrs[i] = addrs[pfd_count - 1];
                        pfd_count--;
                        continue;
                    }
                    int n = atoi(client_messg);
                    unsigned long long int p = fac(n);
                    memset(server_messg, 0, MESG_SIZE);
                    sprintf(server_messg, "%llu", p);
                    send(sock_fd, server_messg, strlen(server_messg), 0);
                    char *ip = get_ip(&addrs[i]);
                    int port = get_port(&addrs[i]);
                    if(PRINT){
                        printf("\n%s\n>>request: %s, Sending response: %s\n\n", SEP, client_messg, server_messg);
                    }
                    if(FILE_PRINT){
                        fprintf(fp, "request: %s, response: %s, IP: %s, Port: %d\n", client_messg, server_messg, ip, port);
                        fflush(fp);
                    }
                }
            }
        }
    }
    close(server_fd);
    return 0;
}
