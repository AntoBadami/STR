#include <stdio.h>
#include <pigpio.h>

#define LED 18
#define PERIODO 500000   // 500 ms

void toggle_led(int LED,int *estado)
{
    *estado = !(*estado);           

    gpioWrite(LED, *estado);    

    if (*estado)
        printf("LED_ON\n");
    else
        printf("LED_OFF\n");
}

int main()
{
    int estado = 0;   // 0 = apagado, 1 = encendido 
    uint32_t tiempo_anterior, tiempo_actual;

    if (gpioInitialise() < 0) {
        printf("Error inicializando pigpio\n");
        return 1;
    }

    gpioSetMode(LED, PI_OUTPUT);

    tiempo_anterior = gpioTick();

    while (1) {
        tiempo_actual = gpioTick();

        if ((tiempo_actual - tiempo_anterior) >= PERIODO)
        {
            toggle_led(LED, &estado);
            tiempo_anterior = tiempo_actual;
        }
    }

    gpioTerminate();
    return 0;
}