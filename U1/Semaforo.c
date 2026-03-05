#include <stdio.h>
#include <unistd.h> 

int main() {

    while (1) {  

        printf("VERDE\n");
        sleep(4);   

        printf("AMARILLO\n");
        sleep(2);   

        printf("ROJO\n");
        sleep(4);   
    }

    return 0;
}