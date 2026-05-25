#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 8081
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len;
    int n;

    // 1. Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // 2. Initialize addresses
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 3. Bind
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(1);
    }

    printf("UDP Server is running on port %d...\n", PORT);

    // 4. Infinite loop (handle multiple clients)
    while (1) {

        addr_len = sizeof(client_addr);  // ⚠️ IMPORTANT: reset every time

        // Receive message
        n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                     (struct sockaddr *)&client_addr, &addr_len);

        if (n < 0) {
            perror("recvfrom failed");
            continue;   // don't exit, keep server alive
        }

        buffer[n] = '\0';

        printf("Client (%s:%d) says: %s\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port),
               buffer);

        // Reply
        char reply[] = "Reply from UDP server";

        if (sendto(sockfd, reply, strlen(reply), 0,
                   (struct sockaddr *)&client_addr, addr_len) < 0) {
            perror("sendto failed");
        }
    }

    // Never reached
    close(sockfd);
    return 0;
}