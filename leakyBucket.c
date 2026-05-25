#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define PACKET_COUNT 3

int main()
{
    srand(time(0));

    int rate, bucket_size;
    int remaining_size = 0;
    int packets[PACKET_COUNT];
    int clk, output, time_to_transmit;
    int i;

    // Generate random packets
    for(i = 0; i < PACKET_COUNT; i++)
    {
        packets[i] = (rand() % 6 + 1) * 10;
    }

    printf("Enter the output rate: ");
    scanf("%d", &rate);

    printf("Enter the bucket size: ");
    scanf("%d", &bucket_size);

    i = 0;

    while(i < PACKET_COUNT || remaining_size > 0)
    {
        if(i < PACKET_COUNT)
        {
            if(packets[i] + remaining_size > bucket_size)
            {
                printf("\nBucket overflow! Packet of size %d dropped\n\n", packets[i]);
            }
            else
            {
                remaining_size += packets[i];
                printf("\nIncoming packet size: %d\n", packets[i]);
                printf("Bytes remaining to transmit: %d\n\n", remaining_size);
            }
            i++;
        }

        // Random transmission time
        time_to_transmit = (rand() % 4 + 1) * 10;
        printf("Time left for transmission: %d units\n\n", time_to_transmit);

        // Transmission process
        for (clk = 0; clk < time_to_transmit; clk += 10)
        {
            sleep(1);

            if (remaining_size > 0)
            {
                if(remaining_size <= rate)
                {
                    output = remaining_size;
                    remaining_size = 0;
                }
                else
                {
                    output = rate;
                    remaining_size -= rate;
                }

                printf("%d bytes transmitted\n", output);
                printf("Bytes remaining to transmit: %d\n\n", remaining_size);
            }
            else
            {
                printf("No packets to transmit!\n\n");
            }
        }
    }

    return 0;
}