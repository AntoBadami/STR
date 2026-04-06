#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define DURACION 10

volatile int running = 1;

long contador_estabilidad = 0;
long contador_navegacion = 0;
long contador_telemetria = 0;

pthread_mutex_t mutex;

void trabajo_pesado() {
    double x = 1.0;
    for (int i = 0; i < 100000; i++) {
        x = x * 1.000001;
    }
}

void* hilo_estabilidad(void* arg) {
    while (running) {

        pthread_mutex_lock(&mutex);

        trabajo_pesado();
        contador_estabilidad++;

        pthread_mutex_unlock(&mutex);
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

        // bloquea el recurso
        pthread_mutex_lock(&mutex);

        usleep(200000); // 200 ms

        trabajo_pesado();
        contador_telemetria++;

        pthread_mutex_unlock(&mutex);

        usleep(300000); // simula periodicidad
    }
    return NULL;
}

void set_fifo(pthread_t thread, int prioridad) {
    struct sched_param param;
    param.sched_priority = prioridad;

    pthread_setschedparam(thread, SCHED_FIFO, &param);
}


int main() {
    pthread_t t1, t2, t3;

    pthread_mutex_init(&mutex, NULL);

    pthread_create(&t1, NULL, hilo_estabilidad, NULL);
    pthread_create(&t2, NULL, hilo_navegacion, NULL);
    pthread_create(&t3, NULL, hilo_telemetria, NULL);

    // Prioridades
    set_fifo(t1, 80); // alta
    set_fifo(t2, 40); // media
    set_fifo(t3, 10); // baja

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
