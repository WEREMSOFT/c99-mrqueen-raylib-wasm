#ifndef __SELECTOR_H__
#define __SELECTOR_H__

#include <raylib.h>
#include <string.h>
#include "../mister_queen/uci.h"
#include "types.h"
#define DEBUG_PRINT
#include "console_output.h"
#include "game_state.h"

static char board_coord_x[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
static char board_coord_y[8] = {'8', '7', '6', '5', '4', '3', '2', '1'};

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

void move_piece(char *coords, game_state_t* game_state)
{
   unsigned int board_index_cource_x = (unsigned int)(strchr(board_coord_x, coords[0]) - board_coord_x);
   unsigned int board_index_cource_y = (unsigned int)(strchr(board_coord_y, coords[1]) - board_coord_y);
   unsigned int board_index_target_x = (unsigned int)(strchr(board_coord_x, coords[2]) - board_coord_x);
   unsigned int board_index_target_y = (unsigned int)(strchr(board_coord_y, coords[3]) - board_coord_y);

   game_state->board[board_index_target_y][board_index_target_x] = game_state->board[board_index_cource_y][board_index_cource_x];
   game_state->board[board_index_cource_y][board_index_cource_x] = EMPTY;
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

void send_move_to_engine(game_state_t* game_state)
{
    char position_as_command[300] = {0};
    char coordinates[5] = {0};
    snprintf(coordinates, 5, "%c%c%c%c", 
        board_coord_x[(int)game_state->selector.position_start.x], 
        board_coord_y[(int)game_state->selector.position_start.z], 
        board_coord_x[(int)game_state->selector.position.x], 
        board_coord_y[(int)game_state->selector.position.z]);

    move_piece(coordinates, game_state);

    snprintf(position_as_command, 300, "position startpos moves %c%c%c%c", 
        board_coord_x[(int)game_state->selector.position_start.x], 
        board_coord_y[(int)game_state->selector.position_start.z], 
        board_coord_x[(int)game_state->selector.position.x], 
        board_coord_y[(int)game_state->selector.position.z]);

    parse_line(position_as_command);
    parse_line(commands[GO]);
}

void selector_process_state_awaiting_target(game_state_t* game_state)
{
    selector_process_keys(&game_state->selector);

    if(IsKeyPressed(KEY_SPACE))
    {
        game_state->selector.state = SELECTOR_STATE_READY;
        send_move_to_engine(game_state);
    }

    DrawCubeWires(game_state->selector.position, 1, 1, 1, RED);
    DrawCubeWires(game_state->selector.position_start, 1, 1, 1, PURPLE);
}

void selector_draw(game_state_t* game_state)
{
    switch(game_state->selector.state){
        case SELECTOR_STATE_READY:
            selector_process_state_ready(&game_state->selector);
            break;
        case SELECTOR_STATE_AWAITING_TARGET:
            selector_process_state_awaiting_target(game_state);
            break;
    }
}
#endif