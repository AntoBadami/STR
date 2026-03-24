#include <stdio.h>
#include <pigpio.h>

#define SERVO 18
#define BOTON 23

#define DEBOUNCE_US 200000  // 200 ms en microsegundos

int posicion = 0; 
uint32_t ultimo_tick = 0;

// convierte grados a microsegundos
int grados_a_pulso(int grados)
{
    return 500 + (grados * 2000 / 180);
}

// ISR del botón
void boton_callback(int gpio, int level, uint32_t tick)
{
    if (level == 1) 
    {
        if ((tick - ultimo_tick) < DEBOUNCE_US)
            return;

        ultimo_tick = tick;

        posicion += 30;

        if (posicion > 180)
            posicion = 0;

        int pulso = grados_a_pulso(posicion);

        gpioServo(SERVO, pulso);

        printf("Posicion: %d grados | Pulso: %d us\n", posicion, pulso);
    }
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

    gpioSetPullUpDown(BOTON, PI_PUD_DOWN);

    int pulso_inicial = grados_a_pulso(0);
    gpioServo(SERVO, pulso_inicial);

    printf("Sistema listo. Servo en 0°\n");

    gpioSetAlertFunc(BOTON, boton_callback);

    while (1)
    {
        time_sleep(1);
    }

    gpioTerminate();
    return 0;
}