#include "logic.h"
#include <unity.h>

int check(int exp, const char* msg)
{
    if (exp == -1)
    {
        perror(msg);
        return -1;
    }
    return exp;
}
