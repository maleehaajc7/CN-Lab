#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/soocket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define PORT 6655
#define SIZE 1024

void receive_response(int sock)
{
    char buffer[SIZE];
    int n;

    while(1)
    {
        memset(buffer, 0 , SIZE);
        n = recv(sock, buffer, SIZE - 1, 0);

        if(n <= 0)
        {
            printf("\nServer disconnected..\n");
            exit(1);
        }

        buffer[n] = '\0';

        char *end = strstr(buffer, "END$");

        if(end)
        {
            *end = '\0';
            printf("%s", buffer);
            break;
        }

        printf("%s", buffer);

    }
}

int main()
{
    int sock;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    char filename[SIZE], cmd[SIZE], line[SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("\nCannot connect");
        exit(1);
    }

    while(1)
    {
        printf("\nEnter command (GET <filename>, PUT <filename> , BYE): ");
        fgets(cmd, SIZE, stdin);

        cmd[strcspn(cmd, "\n")] = '\0';

        if(sscanf(cmd, "PUT %s", filename) == 1)
        {
            FILE *fp = fopen(filename, "r");

            if(!fp)
            {
                perror("\nFile open failed\n");
                continue;
            }

            while(fgets(line, SIZE, fp))
            {
                send(sock, line, strlen(line), 0);
            }

            fclose(fp);

            send(sock, "END$", 4, 0);
            printf("Uploading...]\n");
            receive_response(sock);
        }
        else
        {
            send(sock, cmd, strlen(cmd), 0);

            printf("Server Response: "\n);
            receive_response(sock);

            if(strncmp(cmd, "BYE", 3) == 0)
            {
                printf("Closing..\n");
                break;
            }
        }
    }

    close(sock);
    return 0;

}