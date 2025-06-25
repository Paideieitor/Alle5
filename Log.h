#ifndef _ALLE5_LOG_H
#define _ALLE5_LOG_H

#include <cstdarg>
void Log(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf_s("\n");
}

#endif // _ALLE5_LOG_H
