#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>


#define packetCount 10

typedef struct packet
{
    int id;
    int received;
}Packet;

Packet packets[packetCount];

int main()
{

    for(int i = 0; i < packetCount; ++i)
    {
        packets[i].id = i + 1;
        packets[i].received = 0; 
    }

    char *ip = "127.0.0.1";
    int port = 5570;
    srand(time(0));

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    int n;
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(sockfd < 0)
    {
        perror("[-]Socket eror");
        exit(1);
    }

    memset(&server_addr, '\0', sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if(n < 0)
    {
        perror("Bind error");
        exit(1);
    }

    while(1)
    {
        bzero(buffer, 1024);

        addr_size = sizeof(client_addr);

        recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*)&client_addr, &addr_size);

        sleep(1);

        int packet = atoi(buffer);

        if(packets[packet - 1].received == 1)
        {
            continue;
        }
        else if(rand()%4 == 0)
        {
            printf("Server: Receieved corrupted packet %s. Sending negative acknowledgement\n", buffer);

            sprintf(buffer, "NACK %d", packet);

            sendto(sockfd, buffer, 1024, 0, (struct sockaddr*)&client_addr, addr_size);
        }
        else
        {
            printf("Server: Recieved packet %s. Sending acknowledgement\n", buffer);

            sprintf(buffer, "ACK %d", packet);

            sendto(sockfd, buffer, 1024, 0, (struct sockaddr*)&client_addr, addr_size);

            packets[packet - 1]. received = 1;
        }
    }

    return 0;
}