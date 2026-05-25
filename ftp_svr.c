#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 8089
#define SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    char buffer[SIZE];
    char cmd[SIZE], filename[128];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket failed");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        exit(1);
    }

    printf("FTP Server running on port %d...\n", PORT);

    client_fd = accept(server_fd, (struct sockaddr*)&addr, &addr_len);
    if (client_fd < 0) {
        perror("Accept failed");
        exit(1);
    }

    printf("Client connected.\n");

    while (1) {
        memset(cmd, 0, sizeof(cmd));

        int n = recv(client_fd, cmd, sizeof(cmd) - 1, 0);
        if (n <= 0) break;

        cmd[n] = '\0';

        /* ---------------- GET ---------------- */
        if (sscanf(cmd, "GET %s", filename) == 1) {
            FILE *fp = fopen(filename, "r");

            if (!fp) {
                strcpy(buffer, "404 File Not Found\nEND$");
                send(client_fd, buffer, strlen(buffer), 0);
                continue;
            }

            while (fgets(buffer, SIZE, fp)) {
                send(client_fd, buffer, strlen(buffer), 0);
            }

            send(client_fd, "END$", 4, 0);
            fclose(fp);
        }

        /* ---------------- PUT ---------------- */
        else if (sscanf(cmd, "PUT %s", filename) == 1) {
            FILE *fp = fopen(filename, "w");

            if (!fp) {
                strcpy(buffer, "500 Cannot Create File\nEND$");
                send(client_fd, buffer, strlen(buffer), 0);
                continue;
            }

            while (1) {
                memset(buffer, 0, SIZE);
                n = recv(client_fd, buffer, SIZE - 1, 0);
                if (n <= 0) break;

                buffer[n] = '\0';

                char *end = strstr(buffer, "END$");
                if (end) {
                    *end = '\0';
                    fputs(buffer, fp);
                    break;
                }

                fputs(buffer, fp);
            }

            fclose(fp);
            strcpy(buffer, "200 Upload Successful\nEND$");
            send(client_fd, buffer, strlen(buffer), 0);
        }

        /* ---------------- BYE ---------------- */
        else if (strcmp(cmd, "BYE") == 0) {
            strcpy(buffer, "Goodbye\nEND$");
            send(client_fd, buffer, strlen(buffer), 0);
            break;
        }

        /* ---------------- INVALID ---------------- */
        else {
            strcpy(buffer, "Invalid Command\nEND$");
            send(client_fd, buffer, strlen(buffer), 0);
        }
    }

    close(client_fd);
    close(server_fd);

    printf("Server closed.\n");
    return 0;
}