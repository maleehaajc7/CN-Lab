#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define TIMEOUT 5
#define PACKET_COUNT 5
#define WINDOW_SIZE 3

void send_packet(int sock, struct sockaddr_in server_addr, int packet)
{
    char buffer[1024];
    bzero(buffer, 1024);

    sprintf(buffer, "PKT%d", packet);   // FIXED FORMAT

    printf("Client: Sending %s\n", buffer);

    sendto(sock, buffer, strlen(buffer), 0,
           (struct sockaddr*)&server_addr, sizeof(server_addr));
}

void send_window(int sock, struct sockaddr_in server_addr,
                 int packets[], int start, int end)
{
    for(int i = start; i < end; i++)
    {
        send_packet(sock, server_addr, packets[i]);
    }
}

int main()
{
    char *ip = "127.0.0.1";
    int port = 6655;

    int client_sock;
    struct sockaddr_in server_addr;

    char buffer[1024];
    socklen_t addr_size;

    struct timeval timeout;

    client_sock = socket(AF_INET, SOCK_DGRAM, 0);

    if(client_sock < 0)
    {
        perror("Socket error");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // FIXED: proper IP binding
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    // timeout setup
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;

    setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO,
                &timeout, sizeof(timeout));

    int packets[PACKET_COUNT];

    for(int i = 0; i < PACKET_COUNT; i++)
    {
        packets[i] = i;
    }

    int window_start = 0;
    int window_end = WINDOW_SIZE;

    send_window(client_sock, server_addr, packets,
                window_start, window_end);

    while(window_start < PACKET_COUNT)
    {
        bzero(buffer, 1024);
        addr_size = sizeof(server_addr);

        int n = recvfrom(client_sock, buffer, 1024, 0,
                        (struct sockaddr*)&server_addr, &addr_size);

        if(n < 0)
        {
            printf("Client: Timeout → Resending window\n");
            send_window(client_sock, server_addr,
                        packets, window_start, window_end);
        }
        else
        {
            int ack;

            if(sscanf(buffer, "ACK%d", &ack) == 1)
            {
                printf("Client: Received %s\n", buffer);

                if(ack >= window_start)
                {
                    // ✅ FIXED cumulative sliding
                    window_start = ack + 1;

                    // ✅ Recalculate window
                    window_end = window_start + WINDOW_SIZE;
                    if(window_end > PACKET_COUNT)
                        window_end = PACKET_COUNT;

                    // ✅ Send new window
                    send_window(client_sock, server_addr,
                                packets, window_start, window_end);
                }
                else
                {
                    printf("Client: Duplicate ACK → Resending window\n");
                    send_window(client_sock, server_addr,
                                packets, window_start, window_end);
                }
            }
        }

        sleep(1);
    }

    close(client_sock);
    printf("Client: All packets transmitted successfully\n");

    return 0;
}