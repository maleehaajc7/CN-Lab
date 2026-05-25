#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    char *ip = "127.0.0.1";
    int port = 5566;

    int sock;
    struct sockaddr_in addr;
    char buffer[1024];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("\nSocket failed\n");
        exit(1);
    }

    printf("\nTCP client socket created\n");

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    printf("\nConnected to the server\n");

    bzero(buffer, sizeof(buffer));

    strcpy(buffer, "Hey this is me client");

    send(sock, buffer, strlen(buffer), 0);

    printf("\nClient: %s", buffer);


    bzero(buffer, sizeof(buffer));

    recv(sock, buffer, sizeof(buffer), 0);

    printf("\nFrom Server: %s", buffer);

    close(sock);

    printf("\n\nDisconnected from server\n");

    return 0;
}