#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define DURACION 10

volatile int running = 1;

long contador_estabilidad = 0;
long contador_navegacion = 0;
long contador_telemetria = 0;

void trabajo_pesado() {
    double x = 1.0;
    for (int i = 0; i < 100000; i++) {
        x = x * 1.000001;
    }
}

void* hilo_estabilidad(void* arg) {
    while (running) {
        trabajo_pesado();
        contador_estabilidad++;
    }
    return NULL;
}

void* hilo_navegacion(void* arg) {
    while (running) {
        trabajo_pesado();
        contador_navegacion++;
    }
    return NULL;
}

void* hilo_telemetria(void* arg) {
    while (running) {
        usleep(500000);
        trabajo_pesado();
        contador_telemetria++;
    }
    return NULL;
}

int main() {
    pthread_t t1, t2, t3;

    // SIN prioridades, SIN FIFO
    pthread_create(&t1, NULL, hilo_estabilidad, NULL);
    pthread_create(&t2, NULL, hilo_navegacion, NULL);
    pthread_create(&t3, NULL, hilo_telemetria, NULL);

    sleep(DURACION);

    running = 0;

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    printf("Resultados:\n");
    printf("Estabilidad: %ld\n", contador_estabilidad);
    printf("Navegacion: %ld\n", contador_navegacion);
    printf("Telemetria: %ld\n", contador_telemetria);

    return 0;
}
