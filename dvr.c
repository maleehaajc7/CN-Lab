#include <stdio.h>

#define MAX 10
#define INF 9999

struct node {
    int dist[MAX];
    int next[MAX];
};

int main() {
    int n, i, j, k;
    int cost[MAX][MAX];
    struct node router[MAX];

    printf("Enter number of routers: ");
    scanf("%d", &n);

    printf("Enter cost matrix (use %d for no connection):\n", INF);

    // Input and initialization
    for(i = 0; i < n; i++) {
        for(j = 0; j < n; j++) {
            scanf("%d", &cost[i][j]);

            if(i == j)
                cost[i][j] = 0;

            router[i].dist[j] = cost[i][j];
            router[i].next[j] = (cost[i][j] == INF) ? -1 : j;
        }
    }

    int updated;

    // Distance Vector Algorithm
    do {
        updated = 0;

        for(i = 0; i < n; i++) {
            for(j = 0; j < n; j++) {
                for(k = 0; k < n; k++) {

                    // Proper INF handling
                    if(router[i].dist[k] != INF &&
                       router[k].dist[j] != INF &&
                       router[i].dist[j] > 
                       router[i].dist[k] + router[k].dist[j]) {

                        router[i].dist[j] =
                            router[i].dist[k] + router[k].dist[j];

                        router[i].next[j] = router[i].next[k];

                        updated = 1;
                    }
                }
            }
        }

    } while(updated);

    // Output routing tables
    for(i = 0; i < n; i++) {
        printf("\nRouter %c Routing Table:\n", i + 65);
        printf("Destination\tNext Hop\tDistance\n");

        for(j = 0; j < n; j++) {
            if(router[i].dist[j] == INF)
                printf("%c\t\t-\t\tINF\n", j + 65);
            else
                printf("%c\t\t%c\t\t%d\n",
                       j + 65,
                       (router[i].next[j] == -1) ? '-' : router[i].next[j] + 65,
                       router[i].dist[j]);
        }
    }

    return 0;
}