#ifndef __GAME_STATE_H__
#define __GAME_STATE_H__

#include <raylib.h>
#include <raymath.h>
#include "types.h"
#include "event.h"
#include "command_history.h"
#include "game_board.h"
#include "selector.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include "gui_system.h"

enum game_state_enum {
    GAME_STATE_PLAYING,
    GAME_STATE_ANIMATING,
    GAME_STATE_WON_WHITE,
    GAME_STATE_WON_BLACK,
    GAME_STATE_CASTLING_WHITE_RIGHT,
    GAME_STATE_CASTLING_WHITE_LEFT,
    GAME_STATE_CASTLING_BLACK,
};

typedef struct game_context_t 
{
    int state;
    unsigned int board[8][8];
    char textBoxText[1000];
    Camera3D camera_perspective;
    Camera3D camera_top;
    Light light1;
    Light light2;
    Light light3;
    Shader shader;
    selector_t selector;
    Texture2D background;
    char target_position[5];
    Vector3 piece_to_move_position_actual;
    Vector3 piece_to_move_position_target;
    Vector3 piece_castling_position_actual;
    Vector3 piece_castlint_position_target;
    float piece_to_move_lerp_percentage;
    unsigned int piece_to_move;
} game_context_t;

char commands[COMMAND_COUNT][50] = {
    "uci",
    "ucinewgame",
    "isready",
    "go",
    "stop"
};


void game_pass_to_state_animating(game_context_t* game_context, char* position){
    game_context->state = GAME_STATE_ANIMATING;
    game_context->piece_to_move_lerp_percentage = 0;
    game_context->piece_to_move = game_board_get_piece_at_source(game_context->board, position);
    game_context->piece_to_move_position_actual = game_board_get_source_coordinates_in_world(position);
    game_context->piece_to_move_position_target = game_board_get_target_coordinates_in_world(position);
    game_board_set_piece_at_source(game_context->board, position, PIECE_IN_MOTION);
    strncpy(game_context->target_position, position, 5);
}

void game_pass_to_state_castling_white_right(game_context_t* game_context){
    game_context->state = GAME_STATE_CASTLING_WHITE_RIGHT;
    game_context->piece_to_move_lerp_percentage = 0;

    game_context->piece_to_move_position_actual = game_board_get_source_coordinates_in_world("e1xx");
    game_context->piece_to_move_position_target = game_board_get_target_coordinates_in_world("xxg1");

    game_context->piece_castling_position_actual = game_board_get_source_coordinates_in_world("h1xx");
    game_context->piece_castlint_position_target = game_board_get_target_coordinates_in_world("xxf1");

    game_board_set_piece_at_source(game_context->board, "e1xx", PIECE_IN_MOTION);
    game_board_set_piece_at_source(game_context->board, "f1xx", PIECE_IN_MOTION);
}

void game_pass_to_state_castling_white_left(game_context_t* game_context){
    game_context->state = GAME_STATE_CASTLING_WHITE_LEFT;
    game_context->piece_to_move_lerp_percentage = 0;

    game_context->piece_to_move_position_actual = game_board_get_source_coordinates_in_world("e1xx");
    game_context->piece_to_move_position_target = game_board_get_target_coordinates_in_world("xxc1");

    game_context->piece_castling_position_actual = game_board_get_source_coordinates_in_world("a1xx");
    game_context->piece_castlint_position_target = game_board_get_target_coordinates_in_world("xxd1");

    game_board_set_piece_at_source(game_context->board, "e1xx", PIECE_IN_MOTION);
    game_board_set_piece_at_source(game_context->board, "a1xx", PIECE_IN_MOTION);
}

void game_event_process(game_context_t* game_context){
    event_t event = {0};
    while((event = event_queue_dequeue()).type)
    {
        switch(event.type){
            case EVENT_UI_NEW_GAME:
                parse_line(commands[STOP]);
                parse_line(commands[UCI]);
                parse_line(commands[UCINEWGAME]);
                parse_line((char *)&commands[ISREADY]);
                game_board_reset(game_context->board);
                char *history_buffer = command_history_get_buffer();
                memset(history_buffer, 0, COMMAND_HISTORY_SIZE);
                game_context->state = GAME_STATE_PLAYING;
                selector_pass_to_state_ready(&game_context->selector);
                event_queue_init();
                break;
            case EVENT_RESPONSE:
            {
                printf("position sent %s", event.data);

                if(game_context->state == GAME_STATE_ANIMATING || game_context->state == GAME_STATE_PLAYING){
                    game_pass_to_state_animating(game_context, event.data);
                    command_history_add_command(" ");
                    command_history_add_command(event.data);
                    selector_pass_to_state_ready(&game_context->selector);
                }

            }
            break;
            case EVENT_COMMAND:
                {

                    char command_as_string[300] = {0};
                    snprintf(command_as_string, 
                    300, 
                    "position startpos moves%s %s",
                    command_history_get_buffer(),
                    event.data);
                    parse_line(command_as_string);

                    if(strcmp(event.data, "e1g1") == 0
                        && TWR_W == game_board_get_piece_at_source(game_context->board, "h1xx")
                        && EMPTY == game_board_get_piece_at_source(game_context->board, "f1xx")){
                        game_pass_to_state_castling_white_right(game_context);
                    } else if(strcmp(event.data, "e1c1") == 0
                        && TWR_W == game_board_get_piece_at_source(game_context->board, "a1xx")
                        && EMPTY == game_board_get_piece_at_source(game_context->board, "b1xx")
                        && EMPTY == game_board_get_piece_at_source(game_context->board, "c1xx")
                        && EMPTY == game_board_get_piece_at_source(game_context->board, "d1xx")
                        ){
                        game_pass_to_state_castling_white_left(game_context);
                    } else {
                        game_pass_to_state_animating(game_context, event.data);
                    }

                    parse_line(commands[GO]);
                    command_history_add_command(" ");
                    command_history_add_command(event.data);
                }
                break;
            case EVENT_LOG:
                printf("info: %s", event.data);
                break;
            default:
                printf("Unknow event: %s\n", event.data);
        }
    }
}

void game_draw_pre() {
    ui_pre_frame_update();
}

void game_draw(game_context_t* game_context) {
    BeginDrawing();
    ClearBackground(LIGHTGRAY);
    DrawTexture(game_context->background, 0, 0, WHITE);
    
    BeginMode3D(game_context->camera_perspective);
    {
        game_board_draw(game_context->board);
        selector_draw(game_context->selector);
    }
    EndMode3D();

    BeginMode3D(game_context->camera_top);
    {
        game_board_draw(game_context->board);
        selector_draw(game_context->selector);
    }
    EndMode3D();
}

void game_draw_post() {
    gui_draw();           
    DrawFPS(900, 19);
    EndDrawing();
}

void game_update(game_context_t* game_state)
{
    game_event_process(game_state);
    
    switch (game_state->state){
        case GAME_STATE_PLAYING:
            selector_update(&game_state->selector, game_state->board);
            game_draw_pre();
            game_draw(game_state);
            game_draw_post();
            break;
        case GAME_STATE_WON_WHITE:
            game_draw_pre();
            game_draw(game_state);

            DrawText("CHECK MATE!", 155, HEIGHT/3 + 5, 100, BLACK);
            DrawText("CHECK MATE!", 150, HEIGHT/3, 100, RED);

            DrawText("WHITE WINS", 255, HEIGHT/3 + 105, 70, BLACK);
            DrawText("WHITE WINS", 250, HEIGHT/3 + 100, 70, WHITE);

            game_draw_post();
            break;
        case GAME_STATE_WON_BLACK:
            game_draw_pre();
            game_draw(game_state);

            DrawText("CHECK MATE!", 155, HEIGHT/3 + 5, 100, BLACK);
            DrawText("CHECK MATE!", 150, HEIGHT/3, 100, RED);
            
            DrawText("BLACK WINS", 255, HEIGHT/3 + 105, 70, WHITE);
            DrawText("BLACK WINS", 250, HEIGHT/3 + 100, 70, BLACK);

            game_draw_post();
            break;
        case GAME_STATE_ANIMATING:
            game_state->piece_to_move_lerp_percentage += 0.01;
            game_state->piece_to_move_position_actual = Vector3Lerp(game_state->piece_to_move_position_actual, game_state->piece_to_move_position_target, game_state->piece_to_move_lerp_percentage);
            if(Vector3Distance(game_state->piece_to_move_position_actual, game_state->piece_to_move_position_target) < 0.1){
                    unsigned int piece = game_board_get_piece_at_target(game_state->board, game_state->target_position);
                    
                    game_state->piece_to_move_position_actual = game_state->piece_to_move_position_target;
                    game_board_set_piece_at_target(game_state->board, game_state->target_position, game_state->piece_to_move);
                    game_board_set_piece_at_source(game_state->board, game_state->target_position, EMPTY);
                    game_state->state = GAME_STATE_PLAYING;


                    if(piece == KNG_B){
                        game_state->state = GAME_STATE_WON_WHITE;
                    }
                    if(piece == KNG_W){
                        game_state->state = GAME_STATE_WON_BLACK;
                    }
            }
            game_draw_pre();

            ClearBackground(LIGHTGRAY);
            DrawTexture(game_state->background, 0, 0, WHITE);
            
            BeginMode3D(game_state->camera_perspective);
            {
                game_board_draw(game_state->board);
                game_board_pieces_draw(game_state->piece_to_move, game_state->piece_to_move_position_actual);
                selector_draw(game_state->selector);
            }
            EndMode3D();

            BeginMode3D(game_state->camera_top);
            {
                game_board_draw(game_state->board);
                game_board_pieces_draw(game_state->piece_to_move, game_state->piece_to_move_position_actual);
                selector_draw(game_state->selector);
            }
            EndMode3D();
            game_draw_post(game_state);
            break;

        case GAME_STATE_CASTLING_WHITE_RIGHT:
            game_state->piece_to_move_lerp_percentage += 0.01;
            game_state->piece_to_move_position_actual = Vector3Lerp(game_state->piece_to_move_position_actual, game_state->piece_to_move_position_target, game_state->piece_to_move_lerp_percentage);
            game_state->piece_castling_position_actual = Vector3Lerp(game_state->piece_castling_position_actual, game_state->piece_castlint_position_target, game_state->piece_to_move_lerp_percentage);
            if(Vector3Distance(game_state->piece_to_move_position_actual, game_state->piece_to_move_position_target) < 0.1){
                    
                    game_state->piece_to_move_position_actual = game_state->piece_to_move_position_target;
                    game_board_set_piece_at_target(game_state->board, "xxg1", KNG_W);
                    game_board_set_piece_at_source(game_state->board, "e1xx", EMPTY);
                    game_board_set_piece_at_target(game_state->board, "xxf1", TWR_W);
                    game_board_set_piece_at_source(game_state->board, "h1xx", EMPTY);

                    game_state->state = GAME_STATE_PLAYING;
            }
            game_draw_pre();

            ClearBackground(LIGHTGRAY);
            DrawTexture(game_state->background, 0, 0, WHITE);
            
            BeginMode3D(game_state->camera_perspective);
            {
                game_board_draw(game_state->board);
                game_board_pieces_draw(KNG_W, game_state->piece_to_move_position_actual);
                game_board_pieces_draw(TWR_W, game_state->piece_castling_position_actual);
            }
            EndMode3D();

            BeginMode3D(game_state->camera_top);
            {
                game_board_draw(game_state->board);
                game_board_pieces_draw(game_state->piece_to_move, game_state->piece_to_move_position_actual);
            }
            EndMode3D();
            game_draw_post(game_state);
            break;
        case GAME_STATE_CASTLING_WHITE_LEFT:
            game_state->piece_to_move_lerp_percentage += 0.01;
            game_state->piece_to_move_position_actual = Vector3Lerp(game_state->piece_to_move_position_actual, game_state->piece_to_move_position_target, game_state->piece_to_move_lerp_percentage);
            game_state->piece_castling_position_actual = Vector3Lerp(game_state->piece_castling_position_actual, game_state->piece_castlint_position_target, game_state->piece_to_move_lerp_percentage);
            if(Vector3Distance(game_state->piece_to_move_position_actual, game_state->piece_to_move_position_target) < 0.1){
                    
                    game_state->piece_to_move_position_actual = game_state->piece_to_move_position_target;
                    game_board_set_piece_at_target(game_state->board, "xxc1", KNG_W);
                    game_board_set_piece_at_source(game_state->board, "e1xx", EMPTY);
                    game_board_set_piece_at_target(game_state->board, "xxd1", TWR_W);
                    game_board_set_piece_at_source(game_state->board, "a1xx", EMPTY);

                    game_state->state = GAME_STATE_PLAYING;
            }
            game_draw_pre();

            ClearBackground(LIGHTGRAY);
            DrawTexture(game_state->background, 0, 0, WHITE);
            
            BeginMode3D(game_state->camera_perspective);
            {
                game_board_draw(game_state->board);
                game_board_pieces_draw(KNG_W, game_state->piece_to_move_position_actual);
                game_board_pieces_draw(TWR_W, game_state->piece_castling_position_actual);
            }
            EndMode3D();

            BeginMode3D(game_state->camera_top);
            {
                game_board_draw(game_state->board);
                game_board_pieces_draw(game_state->piece_to_move, game_state->piece_to_move_position_actual);
            }
            EndMode3D();
            game_draw_post();
            break;
    }

}

#endif