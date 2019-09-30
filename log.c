#include <stdarg.h>
#include <stdio.h>
#include "error.h"

int flag_quiet = 0;

void log_printf(char *fmt, ...)
{
    if (flag_quiet) return;

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}
