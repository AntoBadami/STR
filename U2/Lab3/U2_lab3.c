#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <pigpio.h>
#include <time.h>

#define I2C_BUS 1
#define AHT10_ADDR 0x38
#define VENTILADOR_PIN 17

// Estados
#define REPOSO 0
#define ALERTA 1
#define VENTILACION 2

float temperatura_global = 0.0;
pthread_mutex_t mutex_temp;

int estado_actual = REPOSO;
int ventilador_encendido = 0;


float leer_temperatura(int handle)
{
    char cmd[3] = {0xAC, 0x33, 0x00};
    char data[6];

    // Envia comando de medición
    if (i2cWriteDevice(handle, cmd, 3) < 0)
    {
        printf("Error escribiendo AHT10\n");
        return -1;
    }

    // Espera que el sensor procese
    usleep(80000); // 80 ms

    if (i2cReadDevice(handle, data, 6) != 6)
    {
        printf("Error leyendo AHT10\n");
        return -1;
    }

    // Conversión (según datasheet)
    int raw = ((data[3] & 0x0F) << 16) | (data[4] << 8) | data[5];
    float temp = ((raw * 200.0) / 1048576.0) - 50;

    return temp;
}


//tarea A
void* tarea_adquisicion(void* arg)
{
    int handle = *(int*)arg;

    while (1)
    {
        float temp = leer_temperatura(handle);

        pthread_mutex_lock(&mutex_temp);
        temperatura_global = temp;
        pthread_mutex_unlock(&mutex_temp);

        sleep(1);
    }
}

//tarea B
void* tarea_control(void* arg)
{
    uint32_t tiempo_inicio_alerta = 0;
    uint32_t tiempo_inicio_vent = 0;

    while (1)
    {
        float temp;

        pthread_mutex_lock(&mutex_temp);
        temp = temperatura_global;
        pthread_mutex_unlock(&mutex_temp);

        uint32_t now = gpioTick();

        switch (estado_actual)
        {
            case REPOSO:
                if (temp > 30)
                {
                    estado_actual = ALERTA;
                    tiempo_inicio_alerta = now;
                }
                break;

            case ALERTA:
                if (temp > 30)
                {
                    if ((now - tiempo_inicio_alerta) > 60000000) // 60s
                    {
                        estado_actual = VENTILACION;
                        gpioWrite(VENTILADOR_PIN, 1);
                        ventilador_encendido = 1;
                        tiempo_inicio_vent = now;
                    }
                }
                else
                {
                    estado_actual = REPOSO;
                }
                break;

            case VENTILACION:
                if ((now - tiempo_inicio_vent) > 120000000 || temp < 25)
                {
                    estado_actual = REPOSO;
                    gpioWrite(VENTILADOR_PIN, 0);
                    ventilador_encendido = 0;
                }
                break;
        }
        usleep(100000); // 100 ms
    }
}

//tarea c
void* tarea_diagnostico(void* arg)
{
    while (1)
    {
        float temp;

        pthread_mutex_lock(&mutex_temp);
        temp = temperatura_global;
        pthread_mutex_unlock(&mutex_temp);

        printf("Temp: %.2f | Estado: %d | Ventilador: %d\n",
               temp, estado_actual, ventilador_encendido);

        sleep(5);
    }
}


int main()
{
    if (gpioInitialise() < 0)
    {
        printf("Error inicializando GPIO\n");
        return -1;
    }

    int i2c_handle = i2cOpen(I2C_BUS, AHT10_ADDR, 0);
    if (i2c_handle < 0)
    {
        printf("Error abriendo I2C\n");
        return -1;
    }

    char init_cmd[3] = {0xE1, 0x08, 0x00};
    i2cWriteDevice(i2c_handle, init_cmd, 3);
    usleep(10000);

    gpioSetMode(VENTILADOR_PIN, PI_OUTPUT);

    pthread_mutex_init(&mutex_temp, NULL);

    pthread_t th_A, th_B, th_C;

    pthread_create(&th_A, NULL, tarea_adquisicion, &i2c_handle);
    pthread_create(&th_B, NULL, tarea_control, NULL);
    pthread_create(&th_C, NULL, tarea_diagnostico, NULL);

    struct sched_param param;

    // Tarea A - Alta
    param.sched_priority = 80;
    pthread_setschedparam(th_A, SCHED_FIFO, &param);

    // Tarea B - Media
    param.sched_priority = 50;
    pthread_setschedparam(th_B, SCHED_FIFO, &param);

    // Tarea C - Baja
    param.sched_priority = 0;
    pthread_setschedparam(th_C, SCHED_OTHER, &param);

    pthread_join(th_A, NULL);
    pthread_join(th_B, NULL);
    pthread_join(th_C, NULL);

    gpioTerminate();

    return 0;
}
