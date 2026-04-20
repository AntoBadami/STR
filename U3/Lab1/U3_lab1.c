#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <pigpio.h>

typedef struct {
    float x;
    float y;
    float z;
} sensor_data_t;

#define N 5

float buffer_x[N] = {0};
float buffer_y[N] = {0};
float buffer_z[N] = {0};
int index_buffer = 0;

pthread_mutex_t mutex;
float angulo_x = 0.0;

int init_mpu6050(int file) {
    char config[2];
    config[0] = 0x6B;
    config[1] = 0;

    if (write(file, config, 2) != 2) {
        perror("Error inicializando MPU6050");
        return -1;
    }
    return 0;
}

int leer_mpu6050(int file, sensor_data_t *data) {
    char reg = 0x3B;
    char buffer[6];

    if (write(file, &reg, 1) != 1) {
        perror("Error escribiendo direccion");
        return -1;
    }

    if (read(file, buffer, 6) != 6) {
        perror("Error leyendo datos");
        return -1;
    }

    int16_t raw_x = (buffer[0] << 8) | buffer[1];
    int16_t raw_y = (buffer[2] << 8) | buffer[3];
    int16_t raw_z = (buffer[4] << 8) | buffer[5];

    data->x = raw_x / 16384.0;
    data->y = raw_y / 16384.0;
    data->z = raw_z / 16384.0;

    return 0;
}

void* productor(void* arg) {
    mqd_t mq = *(mqd_t*)arg;
    sensor_data_t data;

    int file;
    int addr = 0x68;

    if ((file = open("/dev/i2c-1", O_RDWR)) < 0) {
        perror("Error abriendo I2C");
        pthread_exit(NULL);
    }

    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        perror("Error conectando al sensor");
        pthread_exit(NULL);
    }

    if (init_mpu6050(file) == -1) {
        pthread_exit(NULL);
    }

    while (1) {
        if (leer_mpu6050(file, &data) == -1) {
            continue;
        }

        if (mq_send(mq, (char*)&data, sizeof(data), 0) == -1) {
            continue;
        }

        usleep(10000); // 100 Hz
    }

    return NULL;
}

void* consumidor(void* arg) {
    mqd_t mq = *(mqd_t*)arg;
    sensor_data_t data;

    while (1) {
        if (mq_receive(mq, (char*)&data, sizeof(data), NULL) == -1) {
            perror("Error en mq_receive");
            continue;
        }

        // Guardar en buffers
        buffer_x[index_buffer] = data.x;
        buffer_y[index_buffer] = data.y;
        buffer_z[index_buffer] = data.z;

        float sum_x = 0, sum_y = 0, sum_z = 0;

        for (int i = 0; i < N; i++) {
            sum_x += buffer_x[i];
            sum_y += buffer_y[i];
            sum_z += buffer_z[i];
        }

        float avg_x = sum_x / N;
        float avg_y = sum_y / N;
        float avg_z = sum_z / N;

        index_buffer = (index_buffer + 1) % N;

        // Guardar para el servo (mutex)
        pthread_mutex_lock(&mutex);
        angulo_x = avg_x;
        pthread_mutex_unlock(&mutex);

        // Salida CSV
        printf("%.2f,%.2f,%.2f\n", avg_x, avg_y, avg_z);
        fflush(stdout);
    }

    return NULL;
}


void* actuador(void* arg) {
    int pin = 18; // GPIO

    if (gpioInitialise() < 0) {
        fprintf(stderr, "Error inicializando pigpio\n");
        pthread_exit(NULL);
    }

    gpioSetMode(pin, PI_OUTPUT);

    while (1) {
        float valor;

        pthread_mutex_lock(&mutex);
        valor = angulo_x;
        pthread_mutex_unlock(&mutex);

        int pulsewidth = 1500 + (int)(valor * 500);

        if (pulsewidth < 1000) pulsewidth = 1000;
        if (pulsewidth > 2000) pulsewidth = 2000;

        gpioServo(pin, pulsewidth);

        usleep(20000); // 50 Hz
    }

    gpioTerminate();
    return NULL;
}

int main() {
    srand(time(NULL));

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(sensor_data_t);
    attr.mq_curmsgs = 0;

    mq_unlink("/cola_sensor");

    mqd_t mq = mq_open("/cola_sensor", O_CREAT | O_RDWR, 0644, &attr);

    if (mq == (mqd_t)-1) {
        perror("Error en mq_open");
        exit(1);
    }

    pthread_t hilo_prod, hilo_cons, hilo_act;

    pthread_mutex_init(&mutex, NULL);

    pthread_create(&hilo_prod, NULL, productor, &mq);
    pthread_create(&hilo_cons, NULL, consumidor, &mq);
    pthread_create(&hilo_act, NULL, actuador, NULL);

    pthread_join(hilo_prod, NULL);
    pthread_join(hilo_cons, NULL);
    pthread_join(hilo_act, NULL);

    mq_close(mq);
    mq_unlink("/cola_sensor");

    return 0;
}
