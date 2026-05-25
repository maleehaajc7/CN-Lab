#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8085

int main()
{
    int sock;
    struct sockaddr_in addr;
    socklen_t len;
    char buffer[1024];
    int n;

    sock= socket(AF_INET, SOCK_DGRAM, 0);

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    char req[] = "time";

    len = sizeof(addr);

    sendto(sock, req, strlen(req), 0, (struct sockaddr*)&addr, len);


    n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&addr, &len);

    if(n < 0)
    {
        perror("\nReceive failed");
        close(sock);
        exit(1);
    }

    buffer[n] = '\0';

    printf("\nServer: System time - %s\n", buffer);

    close(sock);
    return 0;

}