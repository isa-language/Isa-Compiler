#include <stdio.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdarg.h>
#include <raylib.h>

void println(const char *__format, ...) {
    va_list args;
    va_start(args, __format);
    vprintf(__format, args);
    va_end(args);
    printf("\n");
}