#include "assert.h"
#include <stdio.h>
#include <Windows.h>

void assert_handle(const char *file, int line) 
{
    system("cls");
    fprintf(stderr, "Assertion failed: %s:%d\n", file, line);
    system("pause");
    exit(1);
}