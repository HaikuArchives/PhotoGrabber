#include <stdarg.h>
#include <stdio.h>

void LogDebug(const char* fmt, ...)
{
    va_list arg_list;
    va_start(arg_list, fmt);
    vprintf(fmt, arg_list);
    va_end(arg_list);
}

void LogError(const char* fmt, ...)
{
    va_list arg_list;
    va_start(arg_list, fmt);
    vprintf(fmt, arg_list);
    va_end(arg_list);
}
