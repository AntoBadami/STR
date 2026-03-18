#include <stdio.h>
#include <pigpio.h>

#define LED 24
#define BOTON 23
//servo 18

// función de interrupción
void boton_callback(int gpio, int level, uint32_t tick)
{
    if (level == 0) {
        gpioWrite(LED, 1);
        printf("BOTON_PRESIONADO\n");
    }
    else if (level == 1) {
        gpioWrite(LED, 0);
        printf("BOTON_LIBERADO\n");
    }
}

int main()
{
    if (gpioInitialise() < 0) {
        printf("Error inicializando pigpio\n");
        return 1;
    }

    gpioSetMode(LED, PI_OUTPUT);
    gpioSetMode(BOTON, PI_INPUT);

    //gpioSetPullUpDown(BOTON, PI_PUD_UP);

    gpioSetAlertFunc(BOTON, boton_callback);

    printf("Esperando eventos del botón...\n");

    while (1) {
        time_sleep(1);
    }

    gpioTerminate();
    return 0;
}