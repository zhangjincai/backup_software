/**
 * power - Calculate the power of number.
 * @param base: Base value.
 * @param exponent: Exponent value.
 *
 * @return base raised to the power exponent.
 */
#include <stdio.h>

double power(double base, int exponent)
{
    //printf("base:%g, exponent:%d\n", base, exponent);

    double value = base;
    int i;

    if (exponent == 0) {
        return 1;
    }
    
    for(i = 1; i < exponent; ++i){
	//printf("i:%d, exponent:%d, value:%g, base:%g\n", i, exponent, value, base);
        value = value * base;
    }
    printf("value:%g\n", value);

    return value;
}
