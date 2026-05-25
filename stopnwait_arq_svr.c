#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT 8085

int main() {
    int sock;
    struct sockaddr_in server, client;
    char packet[1024];
    socklen_t len = sizeof(client);

    int expected_seq = 0;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket error");
        return 1;
    }

    srand(time(NULL));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind error");
        close(sock);
        return 1;
    }

    printf("Server started...\n");

    while (1) {
        int n = recvfrom(sock, packet, sizeof(packet) - 1, 0,
                         (struct sockaddr *)&client, &len);

        if (n < 0) {
            perror("Receive error");
            continue;
        }

        packet[n] = '\0';

        int seq = atoi(packet);  // packet contains sequence number

        // Simulate packet loss
        if (rand() % 3 == 0) {
            printf("Packet %d dropped\n", seq);
            continue;
        }

        if (seq == expected_seq) {
            printf("Received packet %d (correct)\n", seq);

            // Send ACK
            char ack[10];
            ack[0] = seq + '0';
            ack[1] = '\0';

            sendto(sock, ack, strlen(ack), 0,
                   (struct sockaddr *)&client, len);

            printf("ACK %d sent\n", seq);

            expected_seq = 1 - expected_seq; // toggle 0 ↔ 1
        } else {
            printf("Duplicate packet %d received\n", seq);

            // Resend ACK for last received correct packet
            char ack[10];
            ack[0] = (1 - expected_seq) + '0';
            ack[1] = '\0';

            sendto(sock, ack, strlen(ack), 0,
                   (struct sockaddr *)&client, len);

            printf("Re-sent ACK %d\n", 1 - expected_seq);
        }
    }

    close(sock);
    return 0;
}