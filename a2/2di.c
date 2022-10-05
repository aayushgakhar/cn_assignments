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

unsigned long long int fac(int n){
    unsigned long long int p=1;
    while(n>0){
        p*=n;
        n--;
    }
    return p;
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

int main(){
    int server_fd, sock_fd, valread;
    struct sockaddr_in server_addr, client_addr;
    int opt = 1;
    int addrlen;

    
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
 
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    addrlen = sizeof(server_addr);
    if (bind(server_fd, (struct sockaddr*)&server_addr,sizeof(server_addr))< 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    fd_set rdset;
    FD_ZERO(&rdset);
    FD_SET(0, &rdset);
    
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    while (1)
    {

        addrlen = sizeof(client_addr);
        if ((sock_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        // char str2[1000];
        // printf("\nREQUEST FROM %s PORT %d\n", inet_ntop(AF_INET, &client_addr.sin_addr, str2, sizeof(str2)), htons(client_addr.sin_port));
        // printf(str2, sizeof(str2));

        char client_messg[MESG_SIZE] = {0};
        char server_messg[MESG_SIZE] = {0};
        char *request, *str, *lID;
        int maxID = 0;
        while (1)
        {
            memset(client_messg,0, MESG_SIZE*sizeof(client_messg[0]));
            valread = recv(sock_fd, client_messg, MESG_SIZE, 0);
            if (valread == 0)
                break;
            // request = strtok(client_messg, " ");
            request = client_messg;
            printf("\n--------------------\nrequest: %s\n", request);

            long long int response = fac(atoi(request));
            memset(server_messg,0, MESG_SIZE);
            sprintf(server_messg, "%llu", response);
            printf("Sending response: %s\n", server_messg);
            send(sock_fd, server_messg, strlen(server_messg), 0);
        }
        close(sock_fd);
    }
    close(server_fd);
    return 0;
}