
#include "command_history.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static char* command_history = NULL;

static unsigned int command_hostory_size_multiplier = 1;

void command_history_init(){
   command_history = (char *)calloc(COMMAND_HISTORY_SIZE, sizeof(char));
   assert(command_history != NULL && "Error initializing memory");
}

void command_history_add_command(char* command){
    if(strlen(command_history) + strlen(command) > COMMAND_HISTORY_SIZE + command_hostory_size_multiplier)
    {
        command_hostory_size_multiplier++;
        void *command_history_realloc = realloc(command_history, COMMAND_HISTORY_SIZE * command_hostory_size_multiplier);
        assert(command_history != NULL && "Error rezising command history buffer");
        command_history = (char *) command_history_realloc;
    }

    strcat(command_history, command);
}

char* command_history_get_buffer(){
    return command_history;
}

void command_history_fini(){
    free(command_history);
}
