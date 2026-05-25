#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

int running = 1;

void *receive_msg(void *arg) {
    int sock = *(int *)arg;
    char buffer[512];

    while (1) {
        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);

        if (n <= 0) {
            printf("\n[INFO] Disconnected from server\n");
            running = 0;
            break;
        }

        buffer[n] = '\0';
        printf("\n%s", buffer);
        printf("You: ");
        fflush(stdout);
    }

    return NULL;
}

int main(void) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket error");
        exit(1);
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9091);

    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(1);
    }

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Connection error");
        exit(1);
    }

    printf("[INFO] Connected to chat server\n");
    printf("You: ");

    pthread_t tid;
    pthread_create(&tid, NULL, receive_msg, &sock);

    char msg[512];

    while (running) {
        fgets(msg, sizeof(msg), stdin);

        // exit condition (nice improvement)
        if (strncmp(msg, "exit", 4) == 0) {
            break;
        }

        send(sock, msg, strlen(msg), 0);
        printf("You: ");
    }

    close(sock);
    return 0;
}