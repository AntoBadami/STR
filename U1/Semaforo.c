#include <stdio.h>
#include <unistd.h> 

#define VERDE 5
#define AMARILLO 2
#define ROJO 5

int main() {

    while (1) {  

        printf("VERDE\n");
        sleep(VERDE);   

        printf("AMARILLO\n");
        sleep(AMARILLO);   

        printf("ROJO\n");
        sleep(ROJO);   
    }

    return 0;
}