#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8085

int main()
{
    int sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len;
    char buffer[1024];
    int n;

    sock= socket(AF_INET, SOCK_DGRAM, 0);

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("\nBind failed");
        close(sock);
        exit(1);
    }

    printf("Server running on port %d...\n", PORT);

    char reply[1024];

    while(1)
    {
        len = sizeof(client_addr);
        n = recvfrom(sock, buffer, sizeof(buffer - 1), 0 , (struct sockaddr*)&client_addr, &len);

        if(n < 0)
        {
            perror("\nReceive error");
            continue;
        }

        buffer[n] = '\0';

        pid_t pid = fork();

        if(pid < 0)
        {
            perror("\nFOrk failed\n");
            continue;
        }
        
        if(pid == 0)
        {
            time_t now = time(NULL);
            char *time_str = ctime(&now);

            printf("\nSending system time..\n");
            sendto(sock, time_str, strlen(time_str), 0 , (struct sockaddr*)&client_addr, len);

            close(sock);
            exit(0);
        }

        while(waitpid(-1,NULL,WNOHANG) > 0);
    }

    close(sock);

    return 0;
}