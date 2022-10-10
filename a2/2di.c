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
    pid_t pid = getpid();
    printf("pid: %d\n", pid);
    
    int maxfd,status;
    int num_clients = 11;
    int sock_fds[num_clients];
    memset(sock_fds, 0, sizeof(sock_fds));
    struct sockaddr_in addrs[num_clients];
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
    // open file
    FILE *fp;
    fp = fopen("output/2di.txt", "w");
    if (fp == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fd_set rdset;
    addrlen = sizeof(client_addr);
    printf("Waiting for connection...");

    char client_messg[MESG_SIZE] = {0};
    char server_messg[MESG_SIZE] = {0};

    while (1)
    {
        FD_ZERO(&rdset);
        FD_SET(server_fd, &rdset);
        maxfd = server_fd;

        for (int i = 0; i < num_clients; i++){
            if (sock_fds[i] > 0)
                FD_SET(sock_fds[i], &rdset);
            if (sock_fds[i] > maxfd)
                maxfd = sock_fds[i];
        }
        status = select(maxfd + 1, &rdset, NULL, NULL, NULL);

        if(status < 0 && errno != EINTR){
            perror("select");
            // exit(EXIT_FAILURE);
        }

        if (FD_ISSET(server_fd, &rdset))
        {
            
            int sock_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
            if (sock_fd < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            printf("Connection accepted from fd %d %s:%d",sock_fd, get_ip(&client_addr), get_port(&client_addr));
            for (int i = 0; i < num_clients; i++)
            {
                if (sock_fds[i] == 0)
                {
                    sock_fds[i] = sock_fd;
                    addrs[i] = client_addr;
                    break;
                }
            }
        }
        for (int i = 0; i < num_clients; i++)
        {
            if (FD_ISSET(sock_fds[i], &rdset))
            {
                memset(client_messg, 0, sizeof(client_messg));
                valread = read(sock_fds[i], client_messg, MESG_SIZE);
                if (valread < 0)
                {
                    perror("read");
                    exit(EXIT_FAILURE);
                }
                if (valread == 0)
                {
                    printf("Client %d disconnected", sock_fds[i]);
                    close(sock_fds[i]);
                    sock_fds[i] = 0;
                    continue;
                }
                int n = atoi(client_messg);
                unsigned long long int p = fac(n);
                memset(server_messg, 0, sizeof(server_messg));
                sprintf(server_messg, "%llu", p);
                send(sock_fds[i], server_messg, strlen(server_messg), 0);

                char* ip = get_ip(&addrs[i]);
                int port = get_port(&addrs[i]);
                printf("\n%s\n>>request: %s, Sending response: %s\n\n", SEP, client_messg, server_messg);

                fprintf(fp, "\n%s\n>>request: %s, response: %s, IP: %s, Port: %d\n", SEP, client_messg, server_messg, ip, port);
            }
        }
        fflush(fp);
    }
    close(server_fd);
    fflush(fp);
    fclose(fp);
    return 0;
}