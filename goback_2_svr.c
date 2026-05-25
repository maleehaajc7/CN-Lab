#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define PORT 6655

int main()
{
    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    char packet[1024], ack[1024];
    socklen_t addr_size;

    int expected = 0;

    srand(time(0));
    int simulate_loss=1;

    return 0;
}