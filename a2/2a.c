#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080

unsigned long long int fac(int n){
    unsigned long long int p=1;
    while(n>0){
        p*=n;
        n--;
    }
    return p;
}

int main(){
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
 
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
 
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket
         = accept(server_fd, (struct sockaddr*)&address,
                  (socklen_t*)&addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

//     struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&address;
//     struct in_addr ipAddr = pV4Addr->sin_addr;
//     char buffer5[1024] = { 0 };
// //    inet_ntop(AF_INET,pV4Addr->sin_addr,buffer5,1024);
//    printf("%s\n", inet_ntoa(address->sin_addr));
//     printf("%s",buffer5);

    char buffer[10240] = {0};
    char buffer2[10240] = {0};
    char *request, *str, *lID;
    int maxID = 0;
    while(1){
        buffer[0] = '\0';
        valread = read(new_socket,buffer,100);
        request = strtok(buffer, " ");
        if (request == NULL)
        {
            continue;
        }
        printf("\n--------------------\nrequest: %s\n", request);

        long long int response = fac(atoi(request));
        // printf("response: %llu", response);
        buffer2[0] = '\0';

        sprintf(buffer2,"%llu",response);
        printf("Sending response: %s\n", buffer2);
        send(new_socket , buffer2 , strlen(buffer2) , 0 );
    }
    close(server_fd);

    return 0;
}