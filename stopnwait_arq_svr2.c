#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define PORT 8085

int main()
{
    int sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(client_addr);

    char packet[10];

    int expected_seq = 0;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if( sock < 0)
    {
        perror("Sock error");
        exit(1);
    }

    srand(time(0));

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

    printf("\nServer started...\n");

    while(1)
    {
        int n = recvfrom(sock, packet, sizeof(packet) - 1, 0, (struct sockaddr*)&client_addr, &len);

        if(n < 0)
        {
            perror("\nreceive error\n");
            continue;
        }

        packet[n] = '\0';

        int seq = atoi(packet);

        char ack[10];

        if(rand() % 3 == 0)
        {
            printf("Packet %d dropped", seq);
            continue;
        }

        if(seq==expected_seq)
        {
            printf("Received packet %d(correct)\n", seq);

            ack[0] = seq + '0';
            ack[1] = '\0';

            printf("Sending ACK%s\n", ack);
            

            expected_seq = 1- expected_seq;
        }
        else
        {
            printf("Duplicate packet %d received\n", seq);

            ack[0] = (1 - expected_seq) + '0';
            ack[1] = '\0';

            printf("\nResent ack%s", ack);
        }
        sendto(sock, ack, strlen(ack), 0, (struct sockaddr*)&client_addr, len);
    }

    close(sock);
    return 0;
}