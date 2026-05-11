#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/control_led.sock"

// Estado del LED simulado
int led_state = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void set_led(int state) {
    led_state = state;
    if (state)
        printf("[SERVIDOR] LED ENCENDIDO\n");
    else
        printf("[SERVIDOR] LED APAGADO\n");
}

void* handle_client(void* arg) {
    int client_fd = *(int*)arg;
    free(arg);

    char buffer[100] = {0};
    int n = read(client_fd, buffer, sizeof(buffer)-1);

    if (n <= 0) {
        close(client_fd);
        return NULL;
    }

    buffer[n] = '\0';

    printf("[SERVIDOR] Comando recibido: %s\n", buffer);

    pthread_mutex_lock(&mutex);

    if (strncmp(buffer, "ON", 2) == 0) {
        set_led(1);
        write(client_fd, "LED_OK: ON", 10);
    } 
    else if (strncmp(buffer, "OFF", 3) == 0) {
        set_led(0);
        write(client_fd, "LED_OK: OFF", 11);
    } 
    else if (strncmp(buffer, "STATUS", 6) == 0) {
        if (led_state)
            write(client_fd, "LED_OK: ON", 10);
        else
            write(client_fd, "LED_OK: OFF", 11);
    } 
    else {
        write(client_fd, "ERROR: Comando invalido", 24);
    }

    pthread_mutex_unlock(&mutex);

    close(client_fd);
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_un addr;

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(1);
    }

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    // Elimina socket anterior si existe
    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(1);
    }

    printf("[SERVIDOR] Escuchando en %s...\n", SOCKET_PATH);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);

        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        pthread_t tid;
        int* pclient = malloc(sizeof(int));
        *pclient = client_fd;

        pthread_create(&tid, NULL, handle_client, pclient);
        pthread_detach(tid);
    }

    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}
