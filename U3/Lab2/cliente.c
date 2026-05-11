#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/control_led.sock"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s ON | OFF | STATUS\n", argv[0]);
        return 1;
    }

    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        return 1;
    }

    write(sock, argv[1], strlen(argv[1]));

    char buffer[100];
    int n = read(sock, buffer, sizeof(buffer)-1);

    if (n > 0) {
        buffer[n] = '\0';
        printf("[CLIENTE] Respuesta: %s\n", buffer);
    }

    close(sock);
    return 0;
}