#include "console_output.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "event.h"

static char *console_buffer = NULL;
static int pending_commands_count = 0;

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

    char* command = strstr(buffer, "bestmove");

    if(command != NULL) {
        command = index(buffer, ' ');
        command++;
        char coordinates[5] = {0};
        strncpy(coordinates, command, 4);
        event_t event = {0};
        event.type = EVENT_RESPONSE;
        sprintf(event.data, coordinates);
        event_queue(event);
    }
    
    static int lines = 0;
    lines++;
    if(lines > 19) {
        char* s = index(console_buffer, '\n');
        if(s != NULL)
        {
            static int lines = 0;
            lines++;
            if(lines > 19) {
                char* s = index(console_buffer, '\n');
                s++;
                strncpy(console_buffer, s, CONSOLE_BUFFER_SIZE);
            }

            strncat(console_buffer, buffer, CONSOLE_BUFFER_SIZE);
            pending_commands_count++;
        } else {
            printf("no encontro el coso?\n");
        }
    }
    strncat(console_buffer, buffer, console_buffer - rindex(console_buffer,'\n'));
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
