#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define PORT 8085

int main() {
    int sock;
    struct sockaddr_in server;
    char packet[10], ack[10];
    socklen_t len = sizeof(server);

    int seq = 0;  // sequence number (0 or 1)

    struct timeval tv;
    tv.tv_sec = 2;   // timeout = 2 seconds
    tv.tv_usec = 0;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket error");
        return 1;
    }

    // Set timeout
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    printf("Client started...\n");

    for (int i = 1; i <= 5; ) {

        sprintf(packet, "%d", seq);

        printf("Sending packet with seq %d\n", seq);

        sendto(sock, packet, strlen(packet), 0,
               (struct sockaddr *)&server, len);

        int n = recvfrom(sock, ack, sizeof(ack) - 1, 0,
                         (struct sockaddr *)&server, &len);

        if (n < 0) {
            printf("Timeout... Retransmitting seq %d\n", seq);
            continue;
        }

        ack[n] = '\0';

        int ack_seq = atoi(ack);

        if (ack_seq == seq) {
            printf("ACK %d received\n", ack_seq);

            seq = 1 - seq; // toggle sequence
            i++;           // move to next packet
        } else {
            printf("Wrong ACK received: %s\n", ack);
        }
    }

    close(sock);
    return 0;
}