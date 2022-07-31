#include <stdio.h>
#include <stdlib.h>

int callback_func1(int a, int b) { return a + b; }
int callback_func2(int a, int b) { return a * b; }

int primary_func(int (*f)(int, int), int c, int d)
{
    return f(c, d) + f(c + d, c - d);
}

int main() {
    int (*f1)(int, int) = callback_func1;
    int (*f2)(int, int) = callback_func2;

    int flag, res;
    scanf("%d", &flag);
    if (flag > 0) {
        res = primary_func(f1, 3, 4);
    }
    else {
        res = primary_func(f2, 3, 4);
    }
    printf("%d\n", res);
   return EXIT_SUCCESS;
}
