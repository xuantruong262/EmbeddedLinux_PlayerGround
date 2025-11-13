#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv = {0};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(5000);
    inet_pton(AF_INET, "0.0.0.0", &serv.sin_addr);

    connect(sock, (struct sockaddr*)&serv, sizeof(serv));
    char msg[100] = "Hello World";
    while (1) {
        send(sock, msg, strlen(msg), 0);
        printf("Echo: %s\n", msg);
    }

    close(sock);
    return 0;
}