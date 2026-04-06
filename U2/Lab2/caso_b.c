/*
    para que se ejecute en un solo nucleo usar:
    taskset -c 0 sudo ./programa
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define DURACION 10   // segundos

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
        usleep(500000); // 500 ms
        trabajo_pesado();
        contador_telemetria++;
    }
    return NULL;
}

void configurar_hilo(pthread_attr_t *attr, int prioridad) {
    struct sched_param param;

    pthread_attr_init(attr);
    pthread_attr_setschedpolicy(attr, SCHED_FIFO);

    param.sched_priority = prioridad;
    pthread_attr_setschedparam(attr, &param);

    pthread_attr_setinheritsched(attr, PTHREAD_EXPLICIT_SCHED);
}

void set_fifo(pthread_t thread, int prioridad) {
    struct sched_param param;
    param.sched_priority = prioridad;

    if (pthread_setschedparam(thread, SCHED_FIFO, &param) != 0) {
        perror("Error seteando SCHED_FIFO");
    }
}


int main() {
    pthread_t t1, t2, t3;
    pthread_attr_t attr1, attr2, attr3;

    configurar_hilo(&attr1, 80);
    configurar_hilo(&attr2, 40);
    configurar_hilo(&attr3, 10);

    pthread_create(&t1, &attr1, hilo_estabilidad, NULL);
    pthread_create(&t2, &attr2, hilo_navegacion, NULL);
    pthread_create(&t3, &attr3, hilo_telemetria, NULL);

    set_fifo(t1, 80);
    set_fifo(t2, 40);
    set_fifo(t3, 10);

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
