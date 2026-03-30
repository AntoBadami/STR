#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <time.h>

#define BASE_PERIOD_MS 100

long long tiempo_actual_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000LL) + (ts.tv_nsec / 1000000LL);
}

void sumar_ms(struct timespec *t, long ms) {
    t->tv_nsec += ms * 1000000;

    while (t->tv_nsec >= 1000000000) {
        t->tv_nsec -= 1000000000;
        t->tv_sec++;
    }
}

// tareas
void tarea1(long long t) {
    printf("Tarea 1 ejecutada en %lld ms\n", t);
}

void tarea2(long long t) {
    printf("Tarea 2 ejecutada en %lld ms\n", t);
}

void tarea3(long long t) {
    printf("Tarea 3 ejecutada en %lld ms\n", t);
}

int main() {
    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);

    int tick = 0;
    long long inicio = tiempo_actual_ms();

    while (1) {
        // calcular próximo instante
        sumar_ms(&next, BASE_PERIOD_MS);

        // esperar hasta ese instante
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);

        tick++;

        long long ahora = tiempo_actual_ms();
        long long transcurrido = ahora - inicio;

        tarea1(transcurrido);

        if (tick % 3 == 0) {
            tarea2(transcurrido);
        }

        tarea1(transcurrido);

        if (tick % 5 == 0) {
            tarea3(transcurrido);
        }

        tarea1(transcurrido);
        
    }

    return 0;
}
