#ifndef __CONSOLE_OUTPUT_H__
#define __CONSOLE_OUTPUT_H__
#include <stdio.h>
#include <strings.h>

#define CONSOLE_BUFFER_SIZE 1000

static char console_buffer[CONSOLE_BUFFER_SIZE] = {0};

void console_buffer_print(char *);
char* console_buffer_get();

#define printf(f_, ...) {\
            char buffer[CONSOLE_BUFFER_SIZE] = {0};\
            snprintf(buffer, CONSOLE_BUFFER_SIZE, (f_), ##__VA_ARGS__);\
            console_buffer_print(buffer);\
            printf("%s", console_buffer);\
        }

#endif