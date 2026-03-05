#include <stdio.h>
#include <unistd.h> 
#include <sys/select.h>

#define VERDE 5
#define AMARILLO 2
#define ROJO 5

int teclaDisponible() {
    struct timeval tv = {0L, 0L};
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(0, &fds);

    return select(1, &fds, NULL, NULL, &tv);
}

int main() {
    int boton = 0; //0 = boton desactivado, 1 = activado

    printf("Presione b o B para activar el boton\n"); 

    while (1) {  

        printf("VERDE\n");
        for (int i = 0; i < VERDE; i++)
        {
            if (teclaDisponible())
            {
                char c = getchar();
                if (c == 'b' || c == 'B')
                {
                    boton = 1;
                    printf("Boton peatonal presionado\n");
                    break;
                }
            }

            sleep(1);
        }

        printf("AMARILLO\n");
        sleep(AMARILLO);   

        printf("ROJO\n");
        sleep(ROJO);   
    }

    return 0;
}