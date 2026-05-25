#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define PACKET_COUNT 3

int main()
{
    srand(time(0));

    int rate, bucket_size, i, clk, time_to_transmit, output, remaining_size = 0;
    int packets[PACKET_COUNT];

    for(i = 0; i < PACKET_COUNT; i++)
    {
        packets[i] = (rand() % 6 + 1)*10;
    }

    printf("\nEnter the size of bucket: ");
    scanf("%d", &bucket_size);

    printf("\nEnter the output rate: ");
    scanf("%d", &rate);

    i = 0;

    while(i < PACKET_COUNT || remaining_size > 0)
    {
        if(i < PACKET_COUNT)
        {
            if(packets[i]+remaining_size > bucket_size)
            {
                printf("\nBucket Overflow!\nDropping packet of size %d", packets[i]);
            }
            else
            {
                printf("\nIncoming packet size of %d\n", packets[i]);
                remaining_size += packets[i];
                printf("Remaining size of data in bucket: %d", remaining_size);
            }
            i++;
        }

        time_to_transmit = (rand() % 4 + 1)*10;
        printf("\nTime left for transmission: %d", time_to_transmit);

        for(clk = 0; clk < time_to_transmit; clk += 10)
        {
            sleep(1);
            if(remaining_size > 0)
            {
                if(remaining_size < rate)
                {
                    output = remaining_size;
                    remaining_size = 0;
                }
                else
                {
                    output = rate;
                    remaining_size -= rate;
                }

                printf("\nOutput transmitted: %d", output);
                printf("\nRemaining bytes in bucket: %d\n", remaining_size);
            }
            else
            {
                printf("\nNo bytes to transmit!\n");
            }
        }
    }

    return 0;
}