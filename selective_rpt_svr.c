#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6665
#define PACKET_COUNT 5

int main() {
    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    char buffer[1024];
    socklen_t addr_size;

    // Track received packets
    int received[PACKET_COUNT];
    for (int i = 0; i < PACKET_COUNT; i++)
        received[i] = 0;

    srand(time(0));

    server_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_sock < 0) {
        perror("Socket error");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        exit(1);
    }

    printf("Selective Repeat Server running on port %d...\n", PORT);

    while (1) {
        bzero(buffer, sizeof(buffer));
        addr_size = sizeof(client_addr);

        int n = recvfrom(server_sock, buffer, sizeof(buffer), 0,
                         (struct sockaddr*)&client_addr, &addr_size);

        if (n < 0) {
            perror("Receive error");
            continue;
        }

        int pkt;
        sscanf(buffer, "PKT%d", &pkt);

        printf("\nReceived: %s\n", buffer);

        sleep(1); // simulate delay

        // 🔥 Case 1: Simulate packet loss (no ACK/NACK)
        if (rand() % 6 == 0) {
            printf("Simulating packet loss for PKT%d\n", pkt);
            continue;
        }

        // 🔥 Case 2: Simulate corruption → send NACK
        if (rand() % 4 == 0) {
            printf("Packet corrupted: PKT%d → Sending NACK\n", pkt);

            sprintf(buffer, "NACK%d", pkt);
            sendto(server_sock, buffer, strlen(buffer), 0,
                   (struct sockaddr*)&client_addr, addr_size);
            continue;
        }

        // 🔥 Case 3: Normal behavior
        if (!received[pkt]) {
            received[pkt] = 1;
            printf("Accepted PKT%d → Sending ACK\n", pkt);
        } else {
            printf("Duplicate PKT%d → Resending ACK\n", pkt);
        }

        sprintf(buffer, "ACK%d", pkt);
        sendto(server_sock, buffer, strlen(buffer), 0,
               (struct sockaddr*)&client_addr, addr_size);
    }

    close(server_sock);
    return 0;
}