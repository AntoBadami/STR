#include <stdio.h>
#include <pigpio.h>

#define LED 18
#define BOTON 23
#define MUESTRAS 20

uint32_t latencias[MUESTRAS];
int contador = 0;

// función de interrupción
void boton_callback(int gpio, int level, uint32_t tick)
{
    if (level == 1 && contador < MUESTRAS) {

        uint32_t t_encendido = gpioTick();

        gpioWrite(LED, 1);

        uint32_t latencia = t_encendido - tick;

        latencias[contador] = latencia;
        contador++;

        printf("Pulsacion %d -> Latencia: %u us\n", contador, latencia);
    }

    if (level == 0) {
        gpioWrite(LED, 0);
    }
}

void calcular_estadisticas()
{
    uint32_t min = latencias[0];
    uint32_t max = latencias[0];
    uint32_t suma = 0;

    for(int i=0;i<MUESTRAS;i++)
    {
        if(latencias[i] < min)
            min = latencias[i];

        if(latencias[i] > max)
            max = latencias[i];

        suma += latencias[i];
    }

    double promedio = (double)suma / MUESTRAS;
    uint32_t jitter = max - min;

    printf("\n----- RESULTADOS -----\n");
    printf("Latencia minima: %u us\n", min);
    printf("Latencia maxima: %u us\n", max);
    printf("Latencia promedio: %.2f us\n", promedio);
    printf("Jitter: %u us\n", jitter);
}

int main()
{
    if (gpioInitialise() < 0) {
        printf("Error inicializando pigpio\n");
        return 1;
    }

    gpioSetMode(LED, PI_OUTPUT);
    gpioSetMode(BOTON, PI_INPUT);

    gpioSetPullUpDown(BOTON, PI_PUD_DOWN);

    gpioSetAlertFunc(BOTON, boton_callback);

    printf("Realizar %d pulsaciones...\n", MUESTRAS);

    while(contador < MUESTRAS)
    {
        time_sleep(1);
    }

    calcular_estadisticas();

    gpioTerminate();
    return 0;
}