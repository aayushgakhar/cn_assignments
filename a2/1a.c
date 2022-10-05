#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 8080
#define MESG_SIZE 2000

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
    while (i < 20)
    {
        printf("\n--------------------\n");
        
        bzero(client_messg,MESG_SIZE);
        sprintf(client_messg,"%d",i+1);
        printf("message sent: %s\n",client_messg);
        send(socket_fd, client_messg, strlen(client_messg), 0);
        
        // server_messg[0] = '\0';
        bzero(server_messg,MESG_SIZE);
        valread = recv( socket_fd , server_messg, MESG_SIZE,0);
        if(valread==0)break;
        printf("response recieved: %s\n",server_messg);
        i+=1;
    }
    printf("Connection closed...\n");
    close(socket_fd);

    return 0;
}