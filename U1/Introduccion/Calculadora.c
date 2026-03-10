#include <stdio.h>

int main() {
    float x = 0, y = 0, w = 0;
    int a = 0;

    printf("Ingrese dos numeros:\n");
    scanf("%f", &x);
    scanf("%f", &y);

    printf("Opciones de operaciones:\n");
    printf("1. Suma\n");
    printf("2. Resta\n");
    printf("3. Multiplicacion\n");
    printf("4. Division\n");
    printf("Ingrese la operacion a realizar: ");
    scanf("%d", &a);

    switch (a) {
        case 1:
            w = x + y;
            printf("%.2f + %.2f = %.2f\n", x, y, w);
            break;

        case 2:
            w = x - y;
            printf("%.2f - %.2f = %.2f\n", x, y, w);
            break;

        case 3:
            w = x * y;
            printf("%.2f * %.2f = %.2f\n", x, y, w);
            break;

        case 4:
            if (y != 0) {
                w = x / y;
                printf("%.2f / %.2f = %.2f\n", x, y, w);
            } else {
                printf("Error: no se puede dividir por cero\n");
            }
            break;

        default:
            printf("Opcion no valida\n");
            break;
    }

    return 0;
}
