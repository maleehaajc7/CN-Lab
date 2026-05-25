#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 9091
#define MAX_CLIENTS 10

typedef struct {
    int sock;
    int id;
} Client;

Client clients[MAX_CLIENTS];
int count = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg) {
    Client *client = (Client *)arg;
    char msg[512];

    printf("[SERVER] Client %d connected\n", client->id);

    while (1) {
        int n = recv(client->sock, msg, sizeof(msg) - 1, 0);

        if (n <= 0) {
            printf("[SERVER] Client %d disconnected\n", client->id);
            close(client->sock);

            pthread_mutex_lock(&lock);
            for (int i = 0; i < count; i++) {
                if (clients[i].sock == client->sock) {
                    clients[i] = clients[count - 1];
                    count--;
                    break;
                }
            }
            pthread_mutex_unlock(&lock);

            free(client); // free dynamically allocated memory
            break;
        }

        msg[n] = '\0';

        char formatted[600];
        snprintf(formatted, sizeof(formatted), "[Client %d]: %s", client->id, msg);

        pthread_mutex_lock(&lock);
        for (int i = 0; i < count; i++) {
            if (clients[i].sock != client->sock) {
                send(clients[i].sock, formatted, strlen(formatted), 0);
            }
        }
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

int main() {
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) {
        perror("Socket error");
        exit(1);
    }

    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind error");
        exit(1);
    }

    if (listen(server, 5) < 0) {
        perror("Listen error");
        exit(1);
    }

    printf("[SERVER] Listening on port %d...\n", PORT);

    while (1) {
        int clientSock = accept(server, NULL, NULL);
        if (clientSock < 0) {
            perror("Accept error");
            continue;
        }

        pthread_mutex_lock(&lock);

        if (count >= MAX_CLIENTS) {
            printf("[SERVER] Max clients reached\n");
            close(clientSock);
            pthread_mutex_unlock(&lock);
            continue;
        }

        // Add to client list
        clients[count].sock = clientSock;
        clients[count].id = count + 1;

        // Allocate memory for thread argument (IMPORTANT FIX)
        Client *new_client = malloc(sizeof(Client));
        *new_client = clients[count];

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, new_client) != 0) {
            perror("Thread error");
            close(clientSock);
            free(new_client);
        } else {
            pthread_detach(tid);
            count++;
        }

        pthread_mutex_unlock(&lock);
    }

    close(server);
    return 0;
}