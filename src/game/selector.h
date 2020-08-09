#ifndef __SELECTOR_H__
#define __SELECTOR_H__

#include <raylib.h>
#include "../mister_queen/uci.h"
#include "types.h"
#define DEBUG_PRINT
#include "console_output.h"

char commands[COMMAND_COUNT][50] = {
    "uci",
    "ucinewgame",
    "isready",
    "go"
};


enum enum_selector_state {
    SELECTOR_STATE_DISABLED,
    SELECTOR_STATE_READY,
    SELECTOR_STATE_AWAITING_TARGET,
};

typedef struct selector_t {
    Vector3 position;
    Vector3 position_start;
    unsigned int state;
} selector_t;

void selector_process_keys(selector_t *selector)
{
    if (IsKeyPressed(KEY_LEFT))
    {
        selector->position.x -= 1.0f;
    }

    if (IsKeyPressed(KEY_RIGHT))
    {
        selector->position.x += 1.0f;
    }

    if (IsKeyPressed(KEY_UP))
    {
        selector->position.z -= 1.0f;
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        selector->position.z += 1.0f;
    }
}

void selector_process_state_ready(selector_t* selector){
    selector_process_keys(selector);

    if(IsKeyPressed(KEY_SPACE))
    {
        selector->state = SELECTOR_STATE_AWAITING_TARGET;
        selector->position_start = selector->position;
    }

    DrawCubeWires(selector->position, 1, 1, 1, RED);
}

static char board_coord_x[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
static char board_coord_y[8] = {'8', '7', '6', '5', '4', '3', '2', '1'};

void send_move_to_engine(selector_t *selector)
{
    char position_as_command[300] = {0};
    snprintf(position_as_command, 300, "position startpos moves %c%c%c%c", 
        board_coord_x[(int)selector->position_start.x], 
        board_coord_y[(int)selector->position_start.z], 
        board_coord_x[(int)selector->position.x], 
        board_coord_y[(int)selector->position.z]);

    parse_line(position_as_command);
    parse_line(commands[GO]);
}

void selector_process_state_awaiting_target(selector_t* selector)
{
    selector_process_keys(selector);

    if(IsKeyPressed(KEY_SPACE))
    {
        selector->state = SELECTOR_STATE_READY;
        send_move_to_engine(selector);
    }

    DrawCubeWires(selector->position, 1, 1, 1, RED);
    DrawCubeWires(selector->position_start, 1, 1, 1, PURPLE);
}

void selector_draw(selector_t* selector)
{
    switch(selector->state){
        case SELECTOR_STATE_READY:
            selector_process_state_ready(selector);
            break;
        case SELECTOR_STATE_AWAITING_TARGET:
            selector_process_state_awaiting_target(selector);
            break;
    }
}
#endif