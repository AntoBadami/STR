#include <stdio.h>
#include <string.h>

int main() {
    char arr[200];
    int contador = 0;
    int enPalabra = 0;  // 0 = false, 1 = true

    printf("Ingrese una cadena de texto: ");
    fgets(arr, sizeof(arr), stdin);

    printf("%s", arr);

    for (int i = 0; arr[i] != '\0'; i++) {
        if (arr[i] != ' ' && arr[i] != '\n' && enPalabra == 0) {
            contador++;
            enPalabra = 1;
        }
        else if (arr[i] == ' ' || arr[i] == '\n') {
            enPalabra = 0;
        }
    }

    printf("Cantidad de palabras: %d\n", contador);

    return 0;
}