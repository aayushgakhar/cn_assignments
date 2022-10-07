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
#include <sys/epoll.h>

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
    int maxfd,status;
    int pfd_size = 32, pfd_count = 0;
    // struct pollfd sock_fds[pfd_size];
    
    int epoll_fd = epoll_create1(0);
    

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

    pfd_count = 1;
    struct epoll_event ev;
    struct epoll_event evs[100];
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);
    // open file
    FILE *fp;
    fp = fopen("output/2diii.txt", "w");
    if (fp == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    addrlen = sizeof(client_addr);
    printf("Waiting for connection...\n");

    char client_messg[MESG_SIZE] = {0};
    char server_messg[MESG_SIZE] = {0};

    struct epoll_event events[pfd_size];
    int icount = 0,xcount=0;
    while (1)
    {
        fprintf(fp,">>>>%d\n",icount++);
        int epoll_count = epoll_wait(epoll_fd, events, pfd_count, -1);
        fprintf(fp,"epoll_count = %d\n",epoll_count);
        if (epoll_count < 0)
        {
            perror("poll");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < epoll_count; i++)
        {

            if (events[i].data.fd == server_fd)
            {
                int new_socket = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
                if (new_socket < 0)
                {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                struct epoll_event evt;
                evt.events = EPOLLIN;
                evt.data.fd = new_socket;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &evt);
                pfd_count++;
                printf("New connection , socket fd is %d , ip: %s , port : %d\n", new_socket, get_ip(&client_addr), get_port(&client_addr));
            }
            else{
                int rs = getpeername(events[i].data.fd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
                memset(client_messg, 0, MESG_SIZE);
                int valread = read(events[i].data.fd, client_messg, MESG_SIZE);
                if (valread < 0)
                {
                    perror("read");
                    exit(EXIT_FAILURE);
                }
                if (valread == 0)
                {
                    printf("Client disconnected, ip: %s , port : %d\n", get_ip(&client_addr), get_port(&client_addr));
                    close(events[i].data.fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, server_fd, 0);
                    // pfd_count--;
                    continue;
                }
                int n = atoi(client_messg);
                unsigned long long int res = fac(n);
                sprintf(server_messg, "%llu", res);
                int sent = send(events[i].data.fd, server_messg, strlen(server_messg), 0);
                char *ip = get_ip(&client_addr);
                int port = get_port(&client_addr);
                printf("\n%s\n>>request: %s, Sending response: %s\n\n", SEP, client_messg, server_messg);

                fprintf(fp, "\n%s\n>>request: %s, response: %s, IP: %s, Port: %d\n", SEP, client_messg, server_messg, ip, port);
                fflush(fp);
            }
        }
    }
    fflush(fp);
    fclose(fp);
    close(server_fd);
    close(epoll_fd);
    return 0;
}