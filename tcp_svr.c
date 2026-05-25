#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    char *ip = "127.0.0.1";
    int port = 5566;

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    if(server_sock < 0)
    {
        perror("\nSocket failed\n");
        exit(1);
    }

    printf("\nServer socket created\n");

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if(n < 0)
    {
        perror("\nBind error\n");
        exit(1);
    }

    printf("\nBinded to port %d successfully\n", port);

    listen(server_sock, 10);
    printf("\nListening...");

    while(1)
    {
        bzero(buffer, sizeof(buffer));
        addr_size = sizeof(client_addr);

        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);

        printf("\nClient connected..");

        recv(client_sock, buffer, sizeof(buffer), 0);

        printf("\nClient: %s", buffer);

        bzero(buffer, sizeof(buffer));

        strcpy(buffer, "Hello from server!");
        
        send(client_sock, buffer, strlen(buffer), 0);

        close(client_sock);

        printf("\nClient disconnected\n");
    }

    return 0;
}