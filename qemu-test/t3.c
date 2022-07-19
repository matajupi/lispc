#include <stdlib.h>
#include <stdio.h>

int main()
{
    int *arr = calloc(256, sizeof(int));
    *(arr + 3) = 3;
    arr[4] = 10;
    printf("%d\n", *(arr + 4));
    free(arr);
    return arr[3];
}
