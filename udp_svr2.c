#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8081

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

        bzero(buffer, sizeof(buffer));

        n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client_addr, &len);

        if(n < 0)
        {
            perror("Receive error\n");
            continue;
        }

        buffer[n] = '\0';

        printf("\nClient (%s : %d) says: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);

        printf("Server msg: ");
        fgets(reply, sizeof(reply), stdin);

        reply[strcspn(reply, "\n")] = '\0';

        sendto(sock, reply, strlen(reply), 0 , (struct sockaddr*)&client_addr, len);
    }

    close(sock);

    return 0;
}