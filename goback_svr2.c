#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define PORT 6655

int main()
{
    int sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(client_addr);

    char packet[512], ack[512];

    int simulation_loss= 1;

    int expected = 0;

    srand(time(0));

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if(sock < 0)
    {
        perror("\nSocket error");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("\nBind error");
        close(sock);
        exit(1);
    }

    printf("\nGo Back N Server listening to port %d...\n", PORT);

    while(1)
    {
        bzero(packet, sizeof(packet));

        int n = recvfrom(sock, packet, sizeof(packet)-1, 0, (struct sockaddr*)&client_addr, &len);

        if(n < 0)
        {
            perror("\nReceive error");
            continue;
        }

        packet[n] = '\0';

        int seq;

        sscanf(packet, "PKT%d", &seq);

        if(seq == expected)
        {
            printf("Received in order: %s", packet);
            expected++;
        }
        else
        {
            printf("Out of Order Packet: %s, (Expected: %d)", packet, expected);
        }

        //creating acknowledgement
        sprintf(ack, "ACK%d", expected - 1);

        sleep(1); //delay

        //ack loss simulation
        if(simulation_loss && rand() % 3 == 0)
        {
            printf("\nCreating ACK loss simulation for: %s\n", ack);
            continue;
        }

        sendto(sock, ack, strlen(ack), 0, (struct sockaddr*)&client_addr, len);
        printf("Sent: %s", ack);
    }

    close(sock);
    return 0;
}