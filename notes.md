#include "console_output.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

void console_buffer_init()
{
    console_buffer = calloc(sizeof(char), CONSOLE_BUFFER_SIZE);
}

void console_buffer_fini()
{
    free(console_buffer);
}

char* console_buffer_get() 
{
    return console_buffer;
}

void console_buffer_print(char *buffer) {
    assert(console_buffer != NULL && "Console buffer is not properly initialized");
    static int lines = 0;
    lines++;
    // if(lines > 19) {
    //     char *pivot_string = calloc(sizeof(char), CONSOLE_BUFFER_SIZE);
    //     assert(pivot_string != NULL && "Console buffer is not properly initialized");
    //     strcpy(pivot_string, console_buffer);
    //     char* s = index(pivot_string, '\n');
    //     if(s != NULL)
    //     {
    //         s++; // we skip the \n character
    //         unsigned int index = 0;
    //         printf("==>: %s\n=====\n", console_buffer);
    //         while(*s){ // || index < CONSOLE_BUFFER_SIZE){
    //             console_buffer[index] = *s;
    //             s++;
    //             index++;
    //         }
    //         strncpy(console_buffer, s, CONSOLE_BUFFER_SIZE);
    //     } else {
    //         printf("no encontro el coso?\n");
    //     }
    //     free(pivot_string);
    // }
    strncat(console_buffer, buffer, CONSOLE_BUFFER_SIZE);
    pending_commands_count++;
}

int console_buffer_pending_commands()
{
    return pending_commands_count;
}
void console_buffer_clear_pending_commands()
{
    pending_commands_count = 0;
}

void simple_printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char *buffer = malloc(CONSOLE_BUFFER_SIZE);
    vsnprintf(buffer, CONSOLE_BUFFER_SIZE, fmt, args);
    console_buffer_print(buffer);
    free(buffer);
    va_end(args);

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}