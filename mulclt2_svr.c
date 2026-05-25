#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 9091
#define MAX_CLIENTS 10

typedef struct{
    int sock;
    int id;
}Client;

Client clients[MAX_CLIENTS];

int count = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg)
{
    Client *client = (Client *)arg;
    char msg[512];

    printf("\nServer: CLient %d connected", client->id);

    while(1)
    {

        int n = recv(client->sock, msg, sizeof(msg) - 1, 0);

        if(n <= 0)
        {
            printf("\nClient %d disconnected\n", client->id);
            close(client->sock);

            pthread_mutex_lock(&lock);

            for(int i = 0; i < count; i++)
            {
                if(clients[i].sock == client->sock)
                {
                    clients[i] = clients[count - 1];
                    count--;
                    break;
                }
            }
            pthread_mutex_unlock(&lock);

            free(client);
            break;
        }
        
        msg[n] = '\0';

        char formatted[1024];
        snprintf(formatted, sizeof(formatted), "CLient %d: %s", client->id, msg);

        pthread_mutex_lock(&lock);

        for(int i = 0; i < count; i++)
        {
            if(clients[i].sock != client->sock)
            {
                send(clients[i].sock, formatted, strlen(formatted), 0);
            }
        }

        pthread_mutex_unlock(&lock);
    }
    return NULL;

}

int main()
{
    int server_sock, client_sock;
    struct sockaddr_in addr;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    if(server_sock < 0)
    {
        perror("\nSocket creation failed\n");
        exit(1);
    }

    printf("\nServer Socket created succesfully\n");

    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("\nBind error\n");
        exit(1);
    }

    if(listen(server_sock, 5) < 0)
    {
        perror("\nListen error");
        exit(1);
    }

    printf("\nListening in port %d\n", PORT);

    while(1)
    {
        client_sock = accept(server_sock, NULL, NULL);

        if(client_sock < 0)
        {
            perror("\nAccept_error");
            continue;
        }

        pthread_mutex_lock(&lock);

        if(count >= MAX_CLIENTS)
        {
            printf("\nMaximum client exceeded!\n");
            close(client_sock);
            pthread_mutex_unlock(&lock);
            continue;
        }

        //add to client list
        clients[count].sock = client_sock;
        clients[count].id = count+1;

        //allocate memory fro thread argument
        Client *newClient = malloc(sizeof(Client));
        *newClient = clients[count];

        pthread_t tid;
        if ((pthread_create(&tid, NULL, handle_client, newClient)) !=0 ){
            perror("\nThread error!");
            close(client_sock);
            free(newClient);
        }
        else
        {
            pthread_detach(tid);
            count++;
        }

        pthread_mutex_unlock(&lock);

    }

    close(server_sock);

    return 0;
}