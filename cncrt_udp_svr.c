#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8082
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len;
    int n;

    // 1. Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // 2. Setup server
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 3. Bind
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(1);
    }

    printf("Concurrent UDP Time Server running on port %d...\n", PORT);

    addr_len = sizeof(client_addr);

    while (1) {
        // 4. Receive request
        n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                     (struct sockaddr *)&client_addr, &addr_len);

        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }

        buffer[n] = '\0';

        // 5. Fork for concurrency
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            continue;
        }

        // 👶 Child process
        if (pid == 0) {
            time_t now = time(NULL);
            char *time_str = ctime(&now);

            if (sendto(sockfd, time_str, strlen(time_str), 0,
                       (struct sockaddr *)&client_addr, addr_len) < 0) {
                perror("sendto failed");
            }

            close(sockfd);  // child done
            exit(0);
        }

        // 👨 Parent process
        // Clean up zombie processes (non-blocking)
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }

    close(sockfd);
    return 0;
}