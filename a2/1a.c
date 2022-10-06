#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

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

int main(int argc, char const *argv[])
{
    int socket_fd = 0, valread;
    struct sockaddr_in server_addr;
    printf("Setting up socket...\n");
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr)<= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    printf("Connecting to server...\n");
    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    char server_messg[MESG_SIZE] = { 0 };
    char client_messg[MESG_SIZE] = { 0 };
    int i=0;
    printf("Sending...\n");
    int w = 0;
    while (i < 20)
    {
        printf("\n--------------------\n");
        
        memset(client_messg,0,MESG_SIZE);
        sprintf(client_messg,"%d",i+1);
        
        send(socket_fd, client_messg, strlen(client_messg), 0);
        
        // server_messg[0] = '\0';
        memset(server_messg,0,MESG_SIZE);
        valread = recv( socket_fd , server_messg, MESG_SIZE,0);
        if(valread==0)break;
        printf("message sent: %s\n", client_messg);
        printf("response recieved: %s",server_messg);

        char *p;
        int ret = 1;
        if (strtoull(server_messg, &p, 10) == fac(i + 1))
        {
            // print green tick symbol
            printf("\033[0;32m");
            printf(" \u2713\n");
            printf("\033[0m");
            ret = 0;
        }else{
            w++;
        }
        i+=1;
    }
    printf("Total wrong: %d\n",w);
    printf("Connection closed...\n");
    close(socket_fd);

    return 0;
}