#include "console_output.h"

char* console_buffer_get() {
    return console_buffer;
}

void console_buffer_print(char *buffer) {
    static lines = 0;
    lines++;
    if(lines > 19) {
        char* s = index(console_buffer, '\n');
        s++;
        strcpy(console_buffer, s);
    }

    strncat(console_buffer, buffer, CONSOLE_BUFFER_SIZE);\
}