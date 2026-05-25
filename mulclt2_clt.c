#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>

int running = 1;

void *recv_msg(void *arg)
{
    int sock = *(int *)arg;

    char buffer[1024];

    while(1)
    {
        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);

        if(n <= 0)
        {
            printf("\nDisconnected from server");
            close(sock);
            running = 0;
            break;
        }

        buffer[n] = '\0';

        printf("\n%s", buffer);
        printf("You: ");
        fflush(stdout);
    }

    return NULL;
}

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock < 0)
    {
        perror("\nSOcker error!");
        exit(1);
    }

    struct sockaddr_in  addr;

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(9091);

    if(inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0)
    {
        perror("\nInvalid address\n");
        exit(1);
    }

    if(connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("\nCannot connect to server\n");
        exit(1);
    }

    printf("\nConnected to chat server");


    printf("You: ");

    pthread_t tid;
    pthread_create(&tid, NULL, recv_msg, &sock);
    
    char msg[512];

    while(running)
    {
        fgets(msg, sizeof(msg), stdin);

        if(strncmp(msg, "exit", 4) == 0)
        {
            break;
        }

        send(sock, msg, strlen(msg), 0);
        printf("You: ");
    }

    close(sock);

    return 0;
}