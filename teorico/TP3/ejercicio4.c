#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 10

void *funcion_hilo(void *arg) {
    pthread_t tid = pthread_self();

    printf("Hilo ejecutándose - TID: %lu\n", tid);

    return NULL;
}

int main() {
    pthread_t hilos[N];

    for (int i = 0; i < N; i++) {
        if (pthread_create(&hilos[i], NULL, funcion_hilo, NULL) != 0) {
            perror("Error al crear hilo");
            return 1;
        }
    }

    // Espera a que todos los hilos terminen
    for (int i = 0; i < N; i++) {
        pthread_join(hilos[i], NULL);
    }

    return 0;
}