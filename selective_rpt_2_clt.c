#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define PORT 6655
#define PACKET_COUNT 5
#define WINDOW_SIZE 2
#define TIMEOUT 3

void send_packet(int sock, struct sockaddr_in server_addr, int pkt)
{
    char buffer[1024];
    bzero(buffer, 1024);

    sprintf(buffer, "PKT%d", pkt);

    printf("\nClient sending: %s\n", buffer);

    sendto(sock, buffer, 1024, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
}

void send_window(int sock, struct sockaddr_in server_addr, int acked[], int start, int end)
{
    for(int i = start; i < end; i++)
    {
        if(!acked[i])
        {
            send_packet(sock, server_addr, i);
        }
    }
}

int main()
{
    int client_sock;
    struct sockaddr_in server_addr;
    socklen_t addr_size;
    char buffer[1024];

    int acked[PACKET_COUNT] = {0};

    struct timeval tv;

    client_sock = socket(AF_INET, SOCK_DGRAM, 0);

    if(client_sock < 0)
    {
        perror("\nSocket error\n");
        exit(1);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    //timeout setup
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    int base = 0;
    int next = 0;

    printf("\nSelective repeeat client started..\n");

    //send initial window
    while(next < base + WINDOW_SIZE && next < PACKET_COUNT)
    {
        send_packet(client_sock, server_addr, next);
        next++;
    }

    while(base < PACKET_COUNT)
    {
        bzero(buffer, sizeof(buffer));
        addr_size = sizeof(server_addr);

        int n = recvfrom(client_sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &addr_size);

        if(n < 0)
        {
            printf("\nClient Timout -> resending unacked packets\n");
            send_window(client_sock, server_addr, acked, base, next);
        }
        else
        {
            int pkt;
            char type[1024];

            sscanf(buffer, "%s%d", type, &pkt);

            if(strcmp(type, "ACK") == 0)
            {
                printf("Client: Received ACK%d", pkt);
                acked[pkt] = 1;
            }
            else if(strcmp(type, "NACK") == 0)
            {
                printf("Client: Received NACK%d -> Retransmitting\n", pkt);
                send_packet(client_sock, server_addr, pkt);
            }

            //slide window
            while(base < PACKET_COUNT && acked[base])
            {
                base++;
                if(next < PACKET_COUNT)
                {
                    send_packet(client_sock, server_addr, next);
                    next++;
                }
            }
        }

        sleep(1);
    }

    printf("\nAll packet successfully transmitted\n");

    close(client_sock);
    return 0;
}