#ifndef __SELECTOR_H__
#define __SELECTOR_H__

#include <raylib.h>
#include <string.h>
#include <stdbool.h>
#include "../mister_queen/uci.h"
#include "commands.h"
#define DEBUG_PRINT
#include "console_output.h"
#include "game_options.h"
#include "game.h"
#include "game_board.h"
#include "event.h"

enum enum_selector_state {
    SELECTOR_STATE_DISABLED,
    SELECTOR_STATE_READY,
    SELECTOR_STATE_AWAITING_TARGET,
    SELECTOR_STATE_ILLEGAL,
};

typedef struct selector_t {
    Vector3 position;
    Vector3 position_start;
    unsigned int state;
    unsigned int origin_piece;
    Vector2 white_king_position;
    unsigned int board_attacked_positions[8][8];
    Color color;
    bool visible;
} selector_t;

void selector_get_coordinates_as_string(selector_t selector, char *result){
    snprintf(result, 5, "%c%c%c%c", 
        board_coord_x[(int)selector.position_start.x], 
        board_coord_y[(int)selector.position_start.z], 
        board_coord_x[(int)selector.position.x], 
        board_coord_y[(int)selector.position.z]);
}

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

static bool selector_is_absolute_pin(selector_t selector){
    return selector.board_attacked_positions[(int)selector.white_king_position.x][(int)selector.white_king_position.y];
}

static bool selector_is_position_under_attack(selector_t selector, Vector2 position){
    return selector.board_attacked_positions[(int)position.y][(int)position.x];
}

void selector_pass_to_state_illegal(selector_t* selector){
        selector->state = SELECTOR_STATE_ILLEGAL;
        selector->position_start = selector->position;
}

void selector_send_move_to_engine(selector_t selector)
{
    char coordinates[5] = {0};
    selector_get_coordinates_as_string(selector, coordinates);

    event_t event = {0};
    event.type = EVENT_COMMAND;

    sprintf(event.data, "%s", coordinates);
    event_queue_enqueue(event);
}

void selector_pass_to_state_ready(selector_t* selector){
    selector->state = SELECTOR_STATE_READY;
    selector->visible = true;
    selector->color = RED;
}


void selector_process_state_illegal(selector_t* selector){
    static float seconds = 0.0f;
    static int counter = 0;
    
    seconds += GetFrameTime();

    if(seconds > 0.1f){
        seconds = 0;
        selector->visible = !selector->visible;
        counter++;
    }

    if(counter > 10){
        counter = 0;
        selector_pass_to_state_ready(selector);
    }

}

void selector_draw(selector_t selector)
{
    switch(selector.state){
        case SELECTOR_STATE_READY:
            DrawCubeWires(selector.position, 1, 1, 1, selector.color);
            break;
        case SELECTOR_STATE_AWAITING_TARGET:
            DrawCubeWires(selector.position, 1, 1, 1, RED);
            DrawCubeWires(selector.position_start, 1, 1, 1, PURPLE);
            break;
        case SELECTOR_STATE_ILLEGAL:
            if(selector.visible){
                DrawCube(selector.position, 1, 1, 1, RED);
            }
            break;
    }
}

bool selector_is_origin_position_legal(selector_t selector, unsigned int board[8][8]) {
    unsigned int piece = board[(int)(selector.position.z)][(int)(selector.position.x)];
    return  piece > 6 
            && !(!game_options.allow_move_if_king_is_pinned 
                && selector_is_absolute_pin(selector) 
                && piece != KNG_W);
}

bool selector_is_target_position_legal(selector_t selector, unsigned int board[8][8]) {
    if(selector.position_start.x == selector.position.x && selector.position_start.z == selector.position.z)
        return false;

    unsigned int target_piece = board[(int)(selector.position.z)][(int)(selector.position.x)];
    unsigned int source_piece = board[(int)(selector.position_start.z)][(int)(selector.position_start.x)];

    bool is_under_attack = selector_is_position_under_attack(selector, (Vector2){selector.position.x, selector.position.z});
    if(game_options.allow_move_king_if_target_is_under_attack && source_piece == KNG_W && is_under_attack)
        return false;
    
    if(target_piece > 7)
        return false;

    switch (selector.origin_piece)
    {
    case PWN_W:
        if(selector.position_start.z <= selector.position.z)
            return false;
        break;
    default:
        break;
    }

    return true;
}

void selector_process_state_awaiting_target(selector_t* selector, unsigned int board[8][8])
{
    selector_process_keys(selector);

    if(IsKeyPressed(KEY_SPACE))
    {
        if(selector_is_target_position_legal(*selector, board)){
            selector->state = SELECTOR_STATE_DISABLED;
            selector_send_move_to_engine(*selector);
        } else {
            selector_pass_to_state_illegal(selector);
        }
    }
}

void selector_process_state_ready(selector_t* selector, unsigned int board[8][8]) {
    selector_process_keys(selector);

    if(IsKeyPressed(KEY_SPACE))
    {
        if (selector_is_origin_position_legal(*selector, board)) {
            selector->state = SELECTOR_STATE_AWAITING_TARGET;
            selector->position_start = selector->position;
            selector->origin_piece = board[(int)selector->position.z][(int)selector->position.x];
        } else {
            selector_pass_to_state_illegal(selector);
        }
    }
}

void selector_update(selector_t* selector, unsigned int board[8][8], unsigned int board_attacked_positions[8][8])
{
    switch(selector->state){
        case SELECTOR_STATE_READY:
            selector_process_state_ready(selector, board);
            break;
        case SELECTOR_STATE_AWAITING_TARGET:
            selector_process_state_awaiting_target(selector, board);
            break;
        case SELECTOR_STATE_DISABLED:
            break;
        case SELECTOR_STATE_ILLEGAL:
            selector_process_state_illegal(selector);
            break;
    }
}
#endif