#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#define PORT 6655
#define PACKET_COUNT 5

int main()
{
    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];

    int received[PACKET_COUNT] = {0};

    srand(time(0));

    server_sock = socket(AF_INET, SOCK_DGRAM, 0);

    if(server_sock < 0)
    {
        perror("\nSocket Error\n");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("\nBinding Failed!\n");
        exit(1);
    }

    printf("\nSelective Repeat Server running on port %d.....", PORT);

    while(1)
    {
        bzero(buffer, sizeof(buffer));
        addr_size = sizeof(client_addr);

        int n = recvfrom(server_sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_size);

        if(n < 0)
        {
            perror("Receive error");
            continue;
        }

        int pkt;

        sscanf(buffer, "PKT%d", &pkt);
        printf("\nReceived %s\n", buffer);

        sleep(1); //simulate delay


        //cases handling

        //case 1: simulating packet loss: no ACK/NACK
        if(rand()%6 == 0)
        {
            printf("\nSimulating packet loss for PKT%d\n", pkt);
            continue;
        }
        //case 2: Simulate corruption, send Negative ACK
        if(rand()%4 == 0)
        {
            printf("\nPacket Corrupted:PKT%d, Sending NACK\n", pkt);

            sprintf(buffer, "NACK%d", pkt);

            sendto(server_sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, addr_size);

            continue;
        }
        //case 3: Normal behaviour
        if(!received[pkt])
        {
            received[pkt] = 1;
            printf("\nAccepted PKT%d -> Sending ACK\n", pkt);
        }
        else
        {
            printf("\nDuplicate PKT%d -> resending ACK\n", pkt);
        }

        sprintf(buffer, "ACK%d", pkt);
        sendto(server_sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, addr_size);

    }

    close(server_sock);
    return 0;
}