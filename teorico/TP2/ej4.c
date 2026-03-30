#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <time.h>
#include <pigpio.h>

#define BASE_PERIOD_MS 100

#define LED1 17
#define LED2 27
#define LED3 22

// estados de los LEDs
int estado1 = 0;
int estado2 = 0;
int estado3 = 0;

// función para sumar ms
void sumar_ms(struct timespec *t, long ms) {
    t->tv_nsec += ms * 1000000;

    while (t->tv_nsec >= 1000000000) {
        t->tv_nsec -= 1000000000;
        t->tv_sec++;
    }
}

// toggle estilo tu ejemplo
void toggle_led(int pin, int *estado, const char *nombre) {
    *estado = !(*estado);
    gpioWrite(pin, *estado);

    if (*estado)
        printf("%s ON\n", nombre);
    else
        printf("%s OFF\n", nombre);
}

// tareas
void tarea1() {
    toggle_led(LED1, &estado1, "LED1");
}

void tarea2() {
    toggle_led(LED2, &estado2, "LED2");
}

void tarea3() {
    toggle_led(LED3, &estado3, "LED3");
}

int main() {

    if (gpioInitialise() < 0) {
        printf("Error inicializando pigpio\n");
        return 1;
    }

    gpioSetMode(LED1, PI_OUTPUT);
    gpioSetMode(LED2, PI_OUTPUT);
    gpioSetMode(LED3, PI_OUTPUT);

    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);

    int tick = 0;

    while (1) {
        // esperar próximo instante
        sumar_ms(&next, BASE_PERIOD_MS);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);

        tick++;

        // Tarea 1 → cada 100 ms
        tarea1();

        // Tarea 2 → cada 300 ms
        if (tick % 3 == 0) {
            tarea2();
        }

        // Tarea 3 → cada 500 ms
        if (tick % 5 == 0) {
            tarea3();
        }
    }

    gpioTerminate();
    return 0;
}