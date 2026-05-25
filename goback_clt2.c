#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define TIMEOUT 5
#define PACKET_COUNT 5
#define WINDOW_SIZE 3

#define PORT 6655

void send_packet(int sock, struct sockaddr_in addr, int pkt)
{
    char msg[1024];
    bzero(msg, sizeof(msg));

    sprintf(msg, "PKT%d", pkt);

    printf("Sending PKT%d\n", pkt);

    sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)&addr, sizeof(addr));
}

void send_window(int sock, struct sockaddr_in addr, int packets[], int start, int end)
{
    for(int i = start; i < end; i++)
    {
        send_packet(sock, addr, packets[i]);
    }
}

int main()
{
    int sock;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    char buffer[1024];

    int packets[PACKET_COUNT];

    sock = socket(AF_INET, SOCK_DGRAM, 0);


    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    struct timeval tv;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;

    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    for(int i = 0; i < PACKET_COUNT; i++)
    {
        packets[i] = i;
    }

    int start = 0;
    int end = WINDOW_SIZE;

    send_window(sock, addr, packets, start, end);

    while(start < PACKET_COUNT)
    {
        bzero(buffer, sizeof(buffer));

        int n = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &len);

        if(n < 0)
        {
            printf("CLient Time out resending window...\n");
            send_window(sock, addr, packets, start, end);
        }
        else
        {
            int ack;

            if(sscanf(buffer, "ACK%d", &ack) == 1)
            {
                printf("Receieved %s\n", buffer);

                if(ack >= start)
                {
                    start = ack + 1;

                    end = start + WINDOW_SIZE;

                    if(end > PACKET_COUNT)
                        end = PACKET_COUNT;

                    //send new window
                    send_window(sock, addr, packets, start, end);

                }
                else
                {
                    printf("Duplicate %s, resending window..\n", buffer);

                    send_window(sock, addr, packets, start, end);
                }
            }
        }
        sleep(1);
    }

    close(sock);
    return 0;
}