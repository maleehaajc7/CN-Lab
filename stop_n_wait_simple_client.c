#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 8084

int main() {
    int sock;
    struct sockaddr_in server;
    char buffer[1024];
    socklen_t len = sizeof(server);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket error");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    printf("Client started...\n");

    for (int seq = 0; seq < 5; ) {

        // Convert sequence number to string
        sprintf(buffer, "%d", seq);

        printf("Sending Frame: %d\n", seq);

        sendto(sock, buffer, strlen(buffer), 0,
               (struct sockaddr*)&server, len);

        // Set timeout (important for Stop & Wait)
        struct timeval tv;
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        memset(buffer, 0, sizeof(buffer));

        int n = recvfrom(sock, buffer, sizeof(buffer), 0,
                         (struct sockaddr*)&server, &len);

        if (n < 0) {
            printf("Timeout! Resending Frame %d...\n\n", seq);
            continue;  // resend same frame
        }

        int ack = atoi(buffer);

        if (ack == seq) {
            printf("ACK received: %d\n\n", ack);
            seq++;  // move to next frame
        } else {
            printf("Wrong ACK! Resending...\n\n");
        }
    }

    close(sock);
    return 0;
}