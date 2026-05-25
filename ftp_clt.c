#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define PORT 8089
#define SIZE 1024

void receive_response(int sock) {
    char buffer[SIZE];
    int n;

    while (1) {
        memset(buffer, 0, SIZE);
        n = recv(sock, buffer, SIZE - 1, 0);

        if (n <= 0) {
            printf("Server disconnected.\n");
            exit(1);
        }

        buffer[n] = '\0';

        // check END$
        char *end = strstr(buffer, "END$");
        if (end) {
            *end = '\0';
            printf("%s", buffer);
            break;
        }

        printf("%s", buffer);
    }
}

int main() {
    int sock;
    struct sockaddr_in server;
    char cmd[SIZE], filename[100], line[SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket error");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    printf("Connected to FTP Server.\n");

    while (1) {
        printf("\nEnter command (GET <file> / PUT <file> / BYE): ");
        fgets(cmd, SIZE, stdin);

        cmd[strcspn(cmd, "\n")] = 0;  // remove newline

        /* ---------------- PUT ---------------- */
        if (sscanf(cmd, "PUT %s", filename) == 1) {
            FILE *fp = fopen(filename, "r");

            if (!fp) {
                perror("File open failed");
                continue;
            }

            send(sock, cmd, strlen(cmd), 0);

            while (fgets(line, SIZE, fp)) {
                send(sock, line, strlen(line), 0);
            }

            fclose(fp);

            send(sock, "END$", 4, 0);

            printf("Uploading file...\n");
            receive_response(sock);
        }

        /* ---------------- GET + BYE + INVALID ---------------- */
        else {
            send(sock, cmd, strlen(cmd), 0);

            printf("Server response:\n");
            receive_response(sock);

            if (strncmp(cmd, "BYE", 3) == 0)
                break;
        }
    }

    close(sock);
    printf("Disconnected.\n");
    return 0;
}