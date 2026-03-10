#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main() {

    while (1) {
        time_t ahora;
        struct tm *info;

        time(&ahora);               // obtiene tiempo actual
        info = localtime(&ahora);   // convierte a hora local

        printf("\r%02d:%02d:%02d",
               info->tm_hour,
               info->tm_min,
               info->tm_sec);

        fflush(stdout);  // fuerza impresión inmediata
        sleep(1);
    }

    return 0;
}