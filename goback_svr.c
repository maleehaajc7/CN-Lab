#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>

#define PORT 6655

int main()
{
    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    char packet[1024], ack[1024];
    socklen_t addr_size;

    int expected = 0;   // Go-Back-N expected packet

    // For simulation
    srand(time(0));
    int simulate_loss = 1;

    server_sock = socket(AF_INET, SOCK_DGRAM, 0);

    if(server_sock < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&server_addr, '\0', sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(1);
    }

    printf("Go-Back-N Server listening on port %d...\n", PORT);

    while(1)
    {
        bzero(packet, sizeof(packet));
        addr_size = sizeof(client_addr);

        int n = recvfrom(server_sock, packet, sizeof(packet) - 1, 0,
                         (struct sockaddr*)&client_addr, &addr_size);

        if(n < 0)
        {
            perror("Receive error");
            continue;
        }

        packet[n] = '\0';

        int seq;
        sscanf(packet, "PKT%d", &seq);

        // Go-Back-N logic
        if(seq == expected)
        {
            printf("Received in-order: %s\n", packet);
            expected++;
        }
        else
        {
            printf("Out-of-order: %s (Expected PKT%d)\n", packet, expected);
        }

        // Prepare ACK
        sprintf(ack, "ACK%d", expected - 1);

        sleep(1); // simulate delay

        // Controlled ACK loss (≈33%)
        if(simulate_loss && rand() % 3 == 0)
        {
            printf("Simulating ACK loss: %s\n", ack);
            continue;
        }

        // Send ACK
        sendto(server_sock, ack, strlen(ack), 0,
               (struct sockaddr*)&client_addr, addr_size);

        printf("Sent: %s\n", ack);
    }

    close(server_sock);
    return 0;
}