#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>

#define PORT 8080

void *threadFunc(void *arg)
{
    int new_socket = *((int *)arg);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    int valread;
    while (1)
    {
        valread = read(new_socket, buffer, 1024);
        printf("message received: %s", buffer);
    }
}

int main(int argc, char const *argv[])
{

    int n = 10;
    pthread_t ptid[n];
    for (int i = 0; i < n; i++)
    {
        int status;
        status = pthread_create(&ptid, NULL, (void *)main, NULL);
        int sock = 0, valread, client_fd;
        struct sockaddr_in serv_addr;

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Socket creation error \n");
            return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
        {
            printf(
                "\nInvalid address/ Address not supported \n");
            return -1;
        }

        if ((client_fd = connect(sock, (struct sockaddr *)&serv_addr,
                                 sizeof(serv_addr))) < 0)
        {
            printf("\nConnection Failed \n");
            return -1;
        }
        char buffer[1024] = {0};

        char *arr[20];
        char str[100];
        for (int i = 0; i < 20; i++)
        {
            arr[i] = (char *)malloc(sizeof(char) * 5);
            sprintf(arr[i], "%d", i + 1);
            // generate_random_string(arr[i],9);
        }
        int i = 0;
        while (1)
        {
            printf("\n--------------------\n");
            printf("message sent: %s\n", arr[i]);
            send(sock, arr[i], strlen(arr[i]), 0);

            // buffer[0] = '\0';
            bzero(buffer, 1024);
            valread = read(sock, buffer, 10000);
            printf("response recieved: %s\n", buffer);
            i += 1;
            if (i >= 20)
                break;
        }

        return 0;
    }