#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define PORT 8085

int main()
{
    int sock;
    struct sockaddr_in addr;
    socklen_t len =sizeof(addr);

    int seq = 0;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    //error print if

    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    memset(&addr, 0 , sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    printf("\nCLient started..\n");

    char packet[10], ack[10];

    for(int i = 0; i < 5;)
    {
        sprintf(packet, "%d", seq);

        printf("Sending packet %d\n", seq);

        sendto(sock, packet, strlen(packet), 0, (struct sockaddr*)&addr, len);

        int n = recvfrom(sock, ack, sizeof(ack)-1, 0, (struct sockaddr*)&addr, &len);

        if(n <= 0)
        {
            printf("Time Out.. Retransitting packet %d\n", seq);
            continue;
        }

        ack[n] = '\0';

        int ack_seq = atoi(ack);

        if(ack_seq == seq)
        {
            printf("\nAck%d received\n", ack_seq);

            seq = 1-seq;
            i++;
        }
        else
        {
            printf("\nWrong acknowledgement %s received\n", ack);
        }
    }

    close(sock);
    return 0;
}