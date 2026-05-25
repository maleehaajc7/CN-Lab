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
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    char buffer[1024];
    int n;

    sock= socket(AF_INET, SOCK_DGRAM, 0);

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);


    char reply[1024];

    while(1)
    {
        bzero(buffer, sizeof(buffer));

        printf("Enter msg: ");
        fgets(buffer, sizeof(buffer), stdin);

        buffer[strcspn(buffer, "\n")] = '\0';

        sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&addr, len);

        printf("\nMsg sent to server\n");

        len = sizeof(addr);

        int n = recvfrom(sock, reply, sizeof(reply) - 1, 0, (struct sockaddr*)&addr, &len);

        if(n < 0)
        {
            perror("\nreceive failed");
            continue;
        }

        reply[n] = '\0';

        printf("\nServer says: %s\n", reply);
    }


    close(sock);
    return 0;
}