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
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    addrlen = sizeof(server_addr);
    if (bind(server_fd, (struct sockaddr*)&server_addr,sizeof(server_addr))< 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    while(1){
        if (listen(server_fd, 3) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        addrlen = sizeof(client_addr);
        if ((sock_fd= accept(server_fd, (struct sockaddr*)&client_addr,(socklen_t*)&addrlen))< 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

    //     struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&address;
    //     struct in_addr ipAddr = pV4Addr->sin_addr;
    //     char buffer5[1024] = { 0 };
    // //    inet_ntop(AF_INET,pV4Addr->sin_addr,buffer5,1024);
    //    printf("%s\n", inet_ntoa(address->sin_addr));
    //     printf("%s",buffer5);

        char client_messg[10240] = {0};
        char server_messg[10240] = {0};
        char *request, *str, *lID;
        int maxID = 0;
        while(1){
            bzero(client_messg,10240);
            valread = recv(sock_fd,client_messg,10240,0);
            if(valread == 0) break;
            // request = strtok(client_messg, " ");
            request = client_messg;
            printf("\n--------------------\nrequest: %s\n", request);

            long long int response = fac(atoi(request));
            bzero(server_messg,10240);
            sprintf(server_messg,"%llu",response);
            printf("Sending response: %s\n", server_messg);
            send(sock_fd , server_messg , strlen(server_messg) , 0 );
        }
        close(sock_fd);
    }
    close(server_fd);
    return 0;
}