#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("192.168.2.8");
    addr.sin_port = htons(5000);

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 3);

    printf("Server waiting on port 5000...\n");
    int client_fd = accept(server_fd, NULL, NULL);
    printf("Client connected!\n");

    char buffer[1024] = {0};
    while (1) {
        int len = recv(client_fd, buffer, sizeof(buffer), 0);
        if (len <= 0) break;
        printf("Client: %s\n", buffer);
        send(client_fd, buffer, len, 0); // Echo back
    }

    close(client_fd);
    close(server_fd);
    return 0;
}