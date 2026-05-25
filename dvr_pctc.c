#include <stdio.h>

#define MAX 10
#define iNF 9999

struct node{
    int dist[MAX];
    int next[MAX];
};

int main()
{

    int n, i ,j, k;

    int cost[MAX][MAX];
    struct node route[MAX];

    printf("\nEnter the no of nodes: ");
    scanf("%d", &n);

    printf("\nEnter the cost Matrix: \n");

    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            scanf("%d", &cost[i][j]);

            if(i == j) route[i].dist[j] = 0;

            route[i].dist[j] = cost[i][j];
            route[i].next[j] = j;

        }
    }

    int updated;

    do{
        updated = 0;

        for(i = 0; i < n; i++)
        {
            for(j = 0; j < n; j++)
            {
                for(k = 0; k < n; k++)
                {
                    if(route[i].dist[j] > route[i].dist[k] + route[k].dist[j])
                    {
                        route[i].dist[j] = route[i].dist[k] + route[k].dist[j];
                        route[i].next[j] = route[i].next[k];

                        updated = 1;
                    }
                }

            }
        }
    }while(updated);

    for(i = 0; i < n; i++)
    {
        printf("\n\nRouter %c Routing Table: \n", i+65);
        printf("Destination\tNext Hop\tDistance\n");

        for(j = 0; j < n; j++)
        {
            printf("%c\t\t%c\t\t%d\n", j+65, route[i].next[j]+65, route[i].dist[j]);
        }
    }

    
    return 0;
}