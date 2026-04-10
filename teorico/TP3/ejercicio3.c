#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  
#include <pthread.h>

void *funcion_hilo(void *arg) {
    pid_t pid = getpid();              // PID del proceso
    pthread_t tid = pthread_self();   // TID del hilo

    printf("Desde el hilo:\n");
    printf("PID: %d\n", pid);
    printf("TID: %lu\n", tid);

    return NULL;
}

int main() {
    pthread_t hilo;

    if (pthread_create(&hilo, NULL, funcion_hilo, NULL) != 0) {
        perror("Error al crear el hilo");
        return 1;
    }

    // Esperar a que el hilo termine
    pthread_join(hilo, NULL);

    return 0;
}