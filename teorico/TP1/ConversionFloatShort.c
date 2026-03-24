#include <stdio.h>

int main()
{
    float var_float = 0;
    short var_short = 0;

    printf("Ingrese un numero flotante:\n");
    scanf("%f", &var_float);

    var_short = (short) var_float;

    printf("Variable Float: %.2f\n", var_float);
    printf("Variable Short: %d\n", var_short);

    return 0;
}
