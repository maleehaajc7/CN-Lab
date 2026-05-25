#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 6665
#define PACKET_COUNT 5
#define WINDOW_SIZE 2
#define TIMEOUT 3

void send_packet(int sock, struct sockaddr_in server_addr, int pkt)
{
    char buffer[1024];
    sprintf(buffer, "PKT%d", pkt);

    printf("Client: Sending %s\n", buffer);

    sendto(sock, buffer, strlen(buffer), 0,
           (struct sockaddr*)&server_addr, sizeof(server_addr));
}

void send_window(int sock, struct sockaddr_in server_addr,
                 int acked[], int start, int end)
{
    for (int i = start; i < end; i++)
    {
        if (!acked[i])
            send_packet(sock, server_addr, i);
    }
}

int main()
{
    int client_sock;
    struct sockaddr_in server_addr;
    char buffer[1024];
    socklen_t addr_size;

    struct timeval tv;

    int acked[PACKET_COUNT] = {0};

    client_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_sock < 0)
    {
        perror("Socket error");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // timeout setup
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    int base = 0;
    int next = 0;

    printf("Selective Repeat Client started...\n");

    // send initial window
    while (next < base + WINDOW_SIZE && next < PACKET_COUNT)
    {
        send_packet(client_sock, server_addr, next);
        next++;
    }

    while (base < PACKET_COUNT)
    {
        bzero(buffer, sizeof(buffer));
        addr_size = sizeof(server_addr);

        int n = recvfrom(client_sock, buffer, sizeof(buffer), 0,
                 (struct sockaddr*)&server_addr, &addr_size);

        if (n < 0)
        {
            printf("Client: Timeout → Resending unACKed packets\n");
            send_window(client_sock, server_addr, acked, base, next);
        }
        else
        {
            buffer[n] = '\0';

            int pkt;

            if (sscanf(buffer, "ACK%d", &pkt) == 1)
            {
                printf("Client: Received ACK%d\n", pkt);
                acked[pkt] = 1;
            }
            else if (sscanf(buffer, "NACK%d", &pkt) == 1)
            {
                printf("Client: Received NACK%d → Retransmitting\n", pkt);
                send_packet(client_sock, server_addr, pkt);
            }

            // slide window
            while (base < PACKET_COUNT && acked[base])
                base++;

            if (next < PACKET_COUNT)
            {
                send_packet(client_sock, server_addr, next);
                next++;
            }
        }

        sleep(1);
    }

    printf("All packets successfully transmitted!\n");

    close(client_sock);
    return 0;
}