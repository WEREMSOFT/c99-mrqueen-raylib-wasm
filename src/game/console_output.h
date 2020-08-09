#ifndef __CONSOLE_OUTPUT_H__
#define __CONSOLE_OUTPUT_H__
#include <stdio.h>
#include <strings.h>
#include <stdarg.h>

#define CONSOLE_BUFFER_SIZE 1000000
static char *console_buffer = NULL;
static int pending_commands_count = 0;

void console_buffer_init();
void console_buffer_fini();
void console_buffer_print(char *);
char* console_buffer_get();
int console_buffer_pending_commands();
void console_buffer_clear_pending_commands();
void simple_printf(const char* fmt, ...);

#endif
