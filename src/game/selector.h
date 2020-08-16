#ifndef __SELECTOR_H__
#define __SELECTOR_H__

#include <raylib.h>
#include <string.h>
#include <stdbool.h>
#include "../mister_queen/uci.h"
#include "types.h"
#define DEBUG_PRINT
#include "console_output.h"
#include "game_state.h"
#include "game_board.h"
#include "event.h"

enum enum_selector_state {
    SELECTOR_STATE_DISABLED,
    SELECTOR_STATE_READY,
    SELECTOR_STATE_AWAITING_TARGET,
    SELECTOR_STATE_ILLEGAL,
};

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

void selector_process_state_ready(game_state_t* game_state){
    selector_process_keys(&game_state->selector);

    if(IsKeyPressed(KEY_SPACE))
    {
        if(game_board_position_contains_white_piece(game_state)){
            game_state->selector.state = SELECTOR_STATE_AWAITING_TARGET;
            game_state->selector.position_start = game_state->selector.position;
        } else {
            game_state->selector.state = SELECTOR_STATE_ILLEGAL;
            game_state->selector.position_start = game_state->selector.position;
        }
    }

    DrawCubeWires(game_state->selector.position, 1, 1, 1, RED);
}

void get_board_coordinates_as_strig(char *result, selector_t selector){
    snprintf(result, 5, "%c%c%c%c", 
        board_coord_x[(int)selector.position_start.x], 
        board_coord_y[(int)selector.position_start.z], 
        board_coord_x[(int)selector.position.x], 
        board_coord_y[(int)selector.position.z]);
}

void selector_send_move_to_engine(game_state_t* game_state)
{
    char coordinates[5] = {0};
    get_board_coordinates_as_strig(coordinates, game_state->selector);

    event_t event = {0};
    event.type = EVENT_COMMAND;

    sprintf(event.data, "%s", coordinates);
    event_queue(event);
}

void selector_process_state_awaiting_target(game_state_t* game_state)
{
    selector_process_keys(&game_state->selector);

    if(IsKeyPressed(KEY_SPACE))
    {
        game_state->selector.state = SELECTOR_STATE_DISABLED;
        selector_send_move_to_engine(game_state);
    }

    DrawCubeWires(game_state->selector.position, 1, 1, 1, RED);
    DrawCubeWires(game_state->selector.position_start, 1, 1, 1, PURPLE);
}

void selector_pass_to_state_ready(selector_t* selector){
    selector->state = SELECTOR_STATE_READY;
}

void selector_process_state_illegal(selector_t* selector){
    static bool blink = false;
    static float seconds = 0.0f;
    static int counter = 0;
    
    seconds += GetFrameTime();

    if(seconds > 0.1f){
        seconds = 0;
        blink = !blink;
        counter++;
    }

    /*Do something*/
    if(blink){
        DrawCube(selector->position, 1, 1, 1, RED);
    }

    if(counter > 10){
        counter = 0;
        selector_pass_to_state_ready(selector);
    }

}

void selector_draw(game_state_t* game_state)
{
    switch(game_state->selector.state){
        case SELECTOR_STATE_READY:
            selector_process_state_ready(game_state);
            break;
        case SELECTOR_STATE_AWAITING_TARGET:
            selector_process_state_awaiting_target(game_state);
            break;
        case SELECTOR_STATE_DISABLED:
            break;
        case SELECTOR_STATE_ILLEGAL:
            selector_process_state_illegal(&game_state->selector);
            break;
    }
}
#endif