#ifndef __COMMAND_HISTORY_H__
#define __COMMAND_HISTORY_H__
#define COMMAND_HISTORY_SIZE 2000

#include <string.h>

void command_history_init();
void command_history_add_command(char* command);
char* command_history_get_buffer();
void command_history_fini();

#endif