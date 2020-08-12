#ifndef __CONSOLE_OUTPUT_H__
#define __CONSOLE_OUTPUT_H__
#include <stdio.h>
#include <strings.h>
#include <stdarg.h>

#define CONSOLE_BUFFER_SIZE 1000000

char* console_buffer_get();
void simple_printf(const char* fmt, ...);

#endif
