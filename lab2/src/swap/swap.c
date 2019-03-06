#include "swap.h"
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 

void Swap(char *left, char *right)
{
    char *tmp = (char *)malloc(sizeof(char));
    strcpy(tmp, left);
    strcpy(left, right);
    strcpy(right, tmp);
    free(tmp);
}
