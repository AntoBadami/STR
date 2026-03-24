#include <stdio.h>

int main()
{
    float var_float = 0;
    short var_short = 0;

    printf("Ingrese un numero short:\n");
    scanf("%hd", &var_short);

    var_float = (float) var_short;

    printf("Variable Short: %d\n", var_short);
    printf("Variable Float: %.2f\n", var_float);

    return 0;
}
