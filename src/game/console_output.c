#include "console_output.h"
#include <string.h>
char* console_buffer_get() {
    return console_buffer;
}

void console_buffer_print(char *buffer) {
    static int lines = 0;
    lines++;
    if(lines > 19) {
        char* s = index(console_buffer, '\n');
        s++;
        strncpy(console_buffer, s, CONSOLE_BUFFER_SIZE);
    }
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