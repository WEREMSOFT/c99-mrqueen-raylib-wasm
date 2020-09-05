#include "console_output.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "event.h"

static char console_buffer[1000] = {0};

char* console_buffer_get() 
{
    return console_buffer;
}

void simple_printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);


    event_t event = {0};

    event.type = EVENT_LOG;
    
    vsnprintf(event.data, 200, fmt, args);

    event_queue_enqueue(event);

    va_end(args);

    char* command = strstr(event.data, "bestmove");
    if(command != NULL){
        command = index(event.data, ' ');
        command++;
        char coordinates[5] = {0};
        strncpy(coordinates, command, 4);
        event.type = EVENT_RESPONSE;
        sprintf(event.data, "%s", coordinates);
        event_queue_enqueue(event);
    }
}
