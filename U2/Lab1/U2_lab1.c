#include <stdio.h>
#include <pigpio.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>
#include <sys/time.h>

#define SERVO 21
#define BOTON 26
#define LED   20

#define DEBOUNCE_US 200000

int posicion = 0;
int direccion = 1; // 1 sube, -1 baja
int emergencia = 0;

uint32_t ultimo_tick = 0;

// Conversión grados a pulso
int grados_a_pulso(int grados)
{
    return 500 + (grados * 2000 / 180);
}

// ISR botón 
void boton_callback(int gpio, int level, uint32_t tick)
{
    if (level == 1)
    {
        if ((tick - ultimo_tick) < DEBOUNCE_US)
            return;

        ultimo_tick = tick;

        emergencia = !emergencia;

        if (emergencia)
        {
            gpioWrite(LED, 1);
            printf("[ALERTA] Parada de emergencia activada\n");
        }
        else
        {
            gpioWrite(LED, 0);
            printf("[INFO] Sistema reanudado\n");
        }

        fflush(stdout);
    }
}

// Hilo de monitoreo 
void* tarea_boton(void* arg)
{
    struct sched_param param;
    param.sched_priority = 50;

    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

    gpioSetAlertFunc(BOTON, boton_callback);

    while (1)
    {
        time_sleep(1);
    }
}

void handler_timer(int sig)
{
    printf("[TELEMETRIA] Posicion: %d° | Estado: %s\n",
           posicion,
           emergencia ? "ALERTA" : "SEGURO");

    fflush(stdout);
}


int main()
{
    if (gpioInitialise() < 0)
    {
        printf("Error inicializando pigpio\n");
        return 1;
    }

    gpioSetMode(SERVO, PI_OUTPUT);
    gpioSetMode(BOTON, PI_INPUT);
    gpioSetMode(LED, PI_OUTPUT);

    gpioSetPullUpDown(BOTON, PI_PUD_DOWN);

    gpioWrite(LED, 0);

    gpioServo(SERVO, grados_a_pulso(0));

    printf("Sistema iniciado\n");

    pthread_t hilo_boton;
    pthread_create(&hilo_boton, NULL, tarea_boton, NULL);

    struct sigaction sa;
    sa.sa_handler = handler_timer;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    struct itimerval timer;
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer, NULL);

    while (1)
    {
        if (!emergencia)
        {
            posicion += direccion;

            if (posicion >= 180)
                direccion = -1;

            if (posicion <= 0)
                direccion = 1;

            int pulso = grados_a_pulso(posicion);
            gpioServo(SERVO, pulso);
        }

        time_sleep(0.02); // movimiento (~50Hz)
    }

    gpioTerminate();
    return 0;
}
