#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 8084

int main() {
    int sock;
    struct sockaddr_in server, client;
    char buffer[1024];
    socklen_t len = sizeof(client);

    int expected_seq = 0;  // Expected frame number

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket error");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Bind error");
        close(sock);
        return 1;
    }

    printf("Server running...\n");

    while (1) {
        memset(buffer, 0, sizeof(buffer));

        int n = recvfrom(sock, buffer, sizeof(buffer), 0,
                         (struct sockaddr*)&client, &len);

        if (n < 0) {
            perror("Receive error");
            continue;
        }

        int seq = atoi(buffer);

        if (seq == expected_seq) {
            printf("Received Frame: %d (Accepted)\n", seq);
            expected_seq++;  // Move to next expected
        } else {
            printf("Received Frame: %d (Duplicate)\n", seq);
        }

        // Send ACK (always send last received correctly)
        char ack[10];
        sprintf(ack, "%d", seq);

        sendto(sock, ack, strlen(ack), 0,
               (struct sockaddr*)&client, len);

        printf("Sent ACK: %d\n\n", seq);
    }

    close(sock);
    return 0;
}