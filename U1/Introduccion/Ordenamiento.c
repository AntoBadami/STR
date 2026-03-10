#include <stdio.h>
#include <stdlib.h>

int main() {
    int n = 0, opcion = 0, temp = 0;

    printf("Ingrese la cantidad de numeros: ");
    scanf("%d", &n);

    int* numeros = (int*) malloc(n * sizeof(int));

    if (numeros == NULL) {
        printf("Error al reservar memoria\n");
        return 1;
    }

    printf("Ingrese la cadena de numeros:\n");

    for (int i = 0; i < n; i++) {
        scanf("%d", &numeros[i]);
    }

    printf("Numeros ingresados:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", numeros[i]);
    }

    printf("Opciones de ordenamiento:\n");
    printf("1. Ascendente\n");
    printf("2. Descendente\n");
    printf("Ingrese la opcion a realizar: ");
    scanf("%d", &opcion);

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {

            if ( (opcion == 1 && numeros[j] > numeros[j + 1]) ||
                (opcion == 2 && numeros[j] < numeros[j + 1]) ) 
            {
                temp = numeros[j];
                numeros[j] = numeros[j + 1];
                numeros[j + 1] = temp;
            }
        }
    }

    printf("Numeros ordenados:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", numeros[i]);
    }

    free(numeros); 

    return 0;
}