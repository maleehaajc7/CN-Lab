#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/soocket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define PORT 6655
#define SIZE 1024

int main()
{
    int server_sock, client_sock;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    char buffer[SIZE], cmd[SIZE], filename[SIZE];

    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("\nBind error");
        exit(1);
    }

    listen(server_sock, 5);

    printf("\nServer running on port %d\n", PORT);

    client_sock = accept(server_sock, (struct sockaddr*)&addr, &len);

    if(client_sock > 0)
    {
        printf("Client connected\n");
    }

    while(1)
    {
        bzero(cmd, SIZE);

        int n = recv(client_sock, cmd, SIZE - 1, 0);

        if(n < 0)
        {
            perror("Receive error\n");
            break;
        }

        cmd[n] = '\0';

        if(sscanf(cmd, "GET %s", filename) == 1)
        {
            FILE *fp = fopen(filename, "r");

            if(!fp)
            {
                strcpy(buffer, "404 File not found\nEND$");
                send(client_sock, buffer, strlen(buffer), 0);
                continue;
            }

            while(fgets(buffer, SIZE, fp))
            {
                send(client_sock, buffer, strlen(buffer), 0);
            }

            send(client_sock, "END$", 4, 0);
            fclose(fp);
        }
        else if(sscanf(cmd, "PUT %s", filename));
        {
            FILE *fp = fopen(filename, "w");

            if(!fp)
            {
                strcpy(buffer, "500 Cannot create file\nEND$");
                send(client_sock, buffer, strlen(buffer), 0);
                continue;
            }

            while(1)
            {
                memset(buffer, 0, sizeof(buffer));

                n = recv(client_sock, buffer, SIZE-1, 0);

                if(n <= 0) break;

                buffer[n] = '\0';

                char *end = strstr(buffer, "END$");
                
                if(end)
                {
                    *end = '\0';
                    fputs(buffer, fp);
                    break;
                }

                fputs(buffer, fp);
            }

            fclose(fp);
            strcpy(buffer, "200 Uploaded Succesful\nEND$");
            send(client_sock, buffer, strlen(buffer), 0);
        }
        else if(strcmp(cmd, "BYE") == 0);
        {
            strcpy(buffer, "Goodbye\nEND$");
            send(client_sock, buffer, strlen(buffer), 0);
            break;
        }
        else
        {
            strcpy(buffer, "Invalid Command\nEND$");
            send(client_sock, buffer, strlen(buffer), 0);
        }
    }

    close(server_sock);
    close(client_sock);

    return 0;
}