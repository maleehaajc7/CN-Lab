#include <stdio.h>
#include <limits.h>

#define MAX 10
#define INF 9999

int graph[MAX][MAX];
int n;

// Function to find minimum distance node
int minDistance(int dist[], int visited[]) {
    int min = INT_MAX, index = -1;

    for(int i = 0; i < n; i++) {
        if(!visited[i] && dist[i] < min) {
            min = dist[i];
            index = i;
        }
    }
    return index;
}

// Dijkstra Algorithm
void dijkstra(int src) {
    int dist[MAX], visited[MAX] = {0};
    int parent[MAX];

    // Initialization
    for(int i = 0; i < n; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }

    dist[src] = 0;

    // Main loop
    for(int count = 0; count < n - 1; count++) {
        int u = minDistance(dist, visited);

        if(u == -1) break;

        visited[u] = 1;

        for(int v = 0; v < n; v++) {
            if(!visited[v] &&
               graph[u][v] != INF &&
               dist[u] != INF &&
               dist[u] + graph[u][v] < dist[v]) {

                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }

    // Print routing table
    printf("\nRouting Table for Router %c:\n", src + 65);
    printf("Destination\tNext Hop\tDistance\n");

    for(int i = 0; i < n; i++) {
        if(i == src) continue;

        if(dist[i] == INF) {
            printf("%c\t\t-\t\tINF\n", i + 65);
        } else {
            int next = i;

            // Backtrack to find next hop
            while(parent[next] != src && parent[next] != -1) {
                next = parent[next];
            }

            printf("%c\t\t%c\t\t%d\n",
                   i + 65,
                   next + 65,
                   dist[i]);
        }
    }
}

int main() {
    int i, j;

    printf("Enter number of nodes: ");
    scanf("%d", &n);

    printf("Enter cost matrix (use %d for no connection):\n", INF);

    // Input graph
    for(i = 0; i < n; i++) {
        for(j = 0; j < n; j++) {
            scanf("%d", &graph[i][j]);

            if(i == j)
                graph[i][j] = 0;

            if(graph[i][j] == 0 && i != j)
                graph[i][j] = INF;
        }
    }

    // Run for all routers
    for(i = 0; i < n; i++) {
        dijkstra(i);
    }

    return 0;
}