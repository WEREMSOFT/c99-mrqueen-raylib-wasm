#include <raylib.h>
#include <raymath.h>
#include "commands.h"
#include "event.h"
#include "game_board.h"
#include "selector.h"
#include "game_options.h"
#include "gui_system.h"
#include "recording_system.h"
#include "game_context.h"
#define RLIGHTS_IMPLEMENTATION
#include <rlights.h>

game_context_t game_context = {0};
extern game_options_t game_options;

char commands[COMMAND_COUNT][50] = {
    "uci",
    "ucinewgame",
    "isready",
    "go",
    "stop"
};

void calculate_diagonals_to_infinite(int i, int j) {
    int pos_j = j;
    int pos_i = i;
    while(pos_j+1 < 8 && pos_i+1 < 8){
        game_context.selector.board_attacked_positions[++pos_j][++pos_i] = true;
        if(game_context.board[pos_j][pos_i] != EMPTY) break;
    }
    pos_j = j;
    pos_i = i;
    while(pos_j+1 < 8 && pos_i-1 >= 0){
        game_context.selector.board_attacked_positions[++pos_j][--pos_i] = true;
        if(game_context.board[pos_j][pos_i] != EMPTY) break;
    }
    pos_j = j;
    pos_i = i;
    while(pos_j-1 >= 0 && pos_i+1 < 8){
        game_context.selector.board_attacked_positions[--pos_j][++pos_i] = true;
        if(game_context.board[pos_j][pos_i] != EMPTY) break;
    }
    pos_j = j;
    pos_i = i;
    while(pos_j-1 >= 0 && pos_i-1 >= 0){
        game_context.selector.board_attacked_positions[--pos_j][--pos_i] = true;
        if(game_context.board[pos_j][pos_i] != EMPTY) break;
    }
}

void calculate_square_to_infinite(int i, int j) {
    int pos_j = j;
    int pos_i = i;
    while(pos_j+1 < 8){
        game_context.selector.board_attacked_positions[++pos_j][pos_i] = true;
        if(game_context.board[pos_j][pos_i] != EMPTY) break;
    }
    pos_j = j;
    pos_i = i;
    while(pos_j-1 >= 0){
        game_context.selector.board_attacked_positions[--pos_j][pos_i] = true;
        if(game_context.board[pos_j][pos_i] != EMPTY) break;
    }
    pos_j = j;
    pos_i = i;
    while(pos_i+1 < 8){
        game_context.selector.board_attacked_positions[pos_j][++pos_i] = true;
        if(game_context.board[pos_j][pos_i] != EMPTY) break;
    }
    pos_j = j;
    pos_i = i;
    while(pos_i-1 >= 0){
        game_context.selector.board_attacked_positions[pos_j][--pos_i] = true;
        if(game_context.board[pos_j][pos_i] != EMPTY) break;
    }
}

void game_board_calculate_attacked_positions(){
    memset(game_context.selector.board_attacked_positions, 0x00, sizeof(unsigned int) * 64);
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            switch (game_context.board[j][i])
            {
                case KNG_W:
                    game_context.selector.white_king_position.x = j;
                    game_context.selector.white_king_position.y = i;
                    break;
                case QEN_B:
                    calculate_diagonals_to_infinite(i, j);
                    calculate_square_to_infinite(i, j);
                    break;
                case PWN_B:
                    if(j+1 < 8 && i+1 < 8){
                        game_context.selector.board_attacked_positions[j+1][i+1] = true;
                    }
                    if(j+1 < 8 && i-1 >= 0){
                        game_context.selector.board_attacked_positions[j+1][i-1] = true;
                    }
                    break;
                case KGT_B:
                    if(j+2 < 8 && i+1 < 8){
                        game_context.selector.board_attacked_positions[j+2][i+1] = true;
                    }
                    if(j+2 < 8 && i-1 >= 0){
                        game_context.selector.board_attacked_positions[j+2][i-1] = true;
                    }
                    if(j+1 < 8 && i-2 >= 0){
                        game_context.selector.board_attacked_positions[j+1][i-2] = true;
                    }
                    if(j+1 < 8 && i+2 < 8){
                        game_context.selector.board_attacked_positions[j+1][i+2] = true;
                    }

                    if(j-2 >= 0 && i-1 >= 0){
                        game_context.selector.board_attacked_positions[j-2][i-1] = true;
                    }
                    if(j-2 >= 0 && i+1 < 8){
                        game_context.selector.board_attacked_positions[j-2][i+1] = true;
                    }
                    if(j-1 >= 0 && i+2 < 8){
                        game_context.selector.board_attacked_positions[j-1][i+2] = true;
                    }
                    if(j-1 >= 0 && i-2 >= 0){
                        game_context.selector.board_attacked_positions[j-1][i-2] = true;
                    }
                    break;
                case BSP_B:
                    calculate_diagonals_to_infinite(i, j);
                    break;
                case TWR_B:
                    calculate_square_to_infinite(i, j);
                    break;
            }
        }
    }
}

static Camera3D camera_perspective_init()
{
    Camera3D return_value = {0};

    return_value.fovy = 45.0f;
    return_value.target = (Vector3){3.5f, 0.0f, 2.f};
    return_value.type = CAMERA_PERSPECTIVE;
    return_value.up = (Vector3){0, 1.0f, 0};
    return_value.position = (Vector3){3.5f, 6.0f, 13.0f};
    return return_value;
}

static Camera3D camera_top_init()
{
    Camera3D return_value = {0};

    return_value.fovy = 45.0f;
    return_value.target = (Vector3){-20.f, 0.0f, 20.f};
    return_value.type = CAMERA_ORTHOGRAPHIC;
    return_value.up = (Vector3){0, 0, -1.0f};
    return_value.position = (Vector3){-20.f, 20.0f, 20.0f};
    return return_value;
}

game_context_t game_context_init(){
    game_context_t game_context = {0};

    msf_gif_begin(&game_context.gif_state, "match_recording.gif", WIDTH, HEIGHT);

    game_context.background = LoadTexture("assets/background.png");
    game_context.background.width = WIDTH;
    game_context.background.height = HEIGHT;
    game_context.board_dirty = true;
    game_context.camera_top = camera_top_init();
    game_context.camera_perspective = camera_perspective_init();

    return game_context;
}

void game_context_fini() {
    msf_gif_end(&game_context.gif_state);
    UnloadTexture(game_context.background);
    UnloadImage(game_context.image_data);
}

void game_context_pass_to_state_animating(char* position){
    game_context.state = GAME_STATE_ANIMATING;
    game_context.piece_to_move_lerp_percentage = 0;
    game_context.piece_to_move = game_board_get_piece_at_source(game_context.board, position);
    game_context.piece_to_move_position_actual = game_board_get_source_coordinates_in_world(position);
    game_context.piece_to_move_position_target = game_board_get_target_coordinates_in_world(position);
    game_board_set_piece_at_source(game_context.board, position, PIECE_IN_MOTION);
    strncpy(game_context.target_position, position, 5);
}

void game_context_pass_to_state_castling_black_right(){
    game_context.state = GAME_STATE_CASTLING_BLACK_RIGHT;
    game_context.piece_to_move_lerp_percentage = 0;

    game_context.piece_to_move_position_actual = game_board_get_source_coordinates_in_world("e8xx");
    game_context.piece_to_move_position_target = game_board_get_target_coordinates_in_world("xxg8");

    game_context.piece_castling_position_actual = game_board_get_source_coordinates_in_world("h8xx");
    game_context.piece_castlint_position_target = game_board_get_target_coordinates_in_world("xxf8");

    game_board_set_piece_at_source(game_context.board, "e8xx", PIECE_IN_MOTION);
    game_board_set_piece_at_source(game_context.board, "h8xx", PIECE_IN_MOTION);
}

void game_context_pass_to_state_castling_black_left(){
    game_context.state = GAME_STATE_CASTLING_BLACK_LEFT;
    game_context.piece_to_move_lerp_percentage = 0;

    game_context.piece_to_move_position_actual = game_board_get_source_coordinates_in_world("e8xx");
    game_context.piece_to_move_position_target = game_board_get_target_coordinates_in_world("xxc8");

    game_context.piece_castling_position_actual = game_board_get_source_coordinates_in_world("a8xx");
    game_context.piece_castlint_position_target = game_board_get_target_coordinates_in_world("xxd8");

    game_board_set_piece_at_source(game_context.board, "e8xx", PIECE_IN_MOTION);
    game_board_set_piece_at_source(game_context.board, "a8xx", PIECE_IN_MOTION);
}

void game_context_pass_to_state_castling_white_right(){
    game_context.state = GAME_STATE_CASTLING_WHITE_RIGHT;
    game_context.piece_to_move_lerp_percentage = 0;

    game_context.piece_to_move_position_actual = game_board_get_source_coordinates_in_world("e1xx");
    game_context.piece_to_move_position_target = game_board_get_target_coordinates_in_world("xxg1");

    game_context.piece_castling_position_actual = game_board_get_source_coordinates_in_world("h1xx");
    game_context.piece_castlint_position_target = game_board_get_target_coordinates_in_world("xxf1");

    game_board_set_piece_at_source(game_context.board, "e1xx", PIECE_IN_MOTION);
    game_board_set_piece_at_source(game_context.board, "h1xx", PIECE_IN_MOTION);
}

void game_context_pass_to_state_castling_white_left(){
    game_context.state = GAME_STATE_CASTLING_WHITE_LEFT;
    game_context.piece_to_move_lerp_percentage = 0;

    game_context.piece_to_move_position_actual = game_board_get_source_coordinates_in_world("e1xx");
    game_context.piece_to_move_position_target = game_board_get_target_coordinates_in_world("xxc1");

    game_context.piece_castling_position_actual = game_board_get_source_coordinates_in_world("a1xx");
    game_context.piece_castlint_position_target = game_board_get_target_coordinates_in_world("xxd1");

    game_board_set_piece_at_source(game_context.board, "e1xx", PIECE_IN_MOTION);
    game_board_set_piece_at_source(game_context.board, "a1xx", PIECE_IN_MOTION);
}

void game_context_event_process(){
    event_t event = {0};
    while((event = event_queue_dequeue()).type)
    {
        switch(event.type){
            case EVENT_UI_NEW_GAME:
                parse_line(commands[STOP]);
                parse_line(commands[UCI]);
                parse_line(commands[UCINEWGAME]);
                parse_line((char *)&commands[ISREADY]);
                game_board_reset(game_context.board);
                char *history_buffer = command_history_get_buffer();
                memset(history_buffer, 0, COMMAND_HISTORY_SIZE);
                game_context.state = GAME_STATE_PLAYING;
                selector_pass_to_state_ready(&game_context.selector);
                event_queue_init();
                game_context.board_dirty = true;
                break;
            case EVENT_RESPONSE:
            {
                printf("position sent %s", event.data);
                
                if(strcmp(event.data, "e8g8") == 0
                    && TWR_B == game_board_get_piece_at_source(game_context.board, "h8xx")
                    && EMPTY == game_board_get_piece_at_source(game_context.board, "f8xx")){
                    game_context_pass_to_state_castling_black_right();
                } else if(strcmp(event.data, "e8c8") == 0
                    && TWR_B == game_board_get_piece_at_source(game_context.board, "a8xx")
                    && EMPTY == game_board_get_piece_at_source(game_context.board, "b8xx")
                    && EMPTY == game_board_get_piece_at_source(game_context.board, "c8xx")
                    && EMPTY == game_board_get_piece_at_source(game_context.board, "d8xx")
                    ){
                    game_context_pass_to_state_castling_black_left();
                } else {
                    if(game_context.state == GAME_STATE_ANIMATING || game_context.state == GAME_STATE_PLAYING){
                        game_context_pass_to_state_animating(event.data);
                        command_history_add_command(" ");
                        command_history_add_command(event.data);
                        selector_pass_to_state_ready(&game_context.selector);
                        game_context.board_dirty = true;
                    }
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
                        && TWR_W == game_board_get_piece_at_source(game_context.board, "h1xx")
                        && EMPTY == game_board_get_piece_at_source(game_context.board, "f1xx")){
                        game_context_pass_to_state_castling_white_right();
                    } else if(strcmp(event.data, "e1c1") == 0
                        && TWR_W == game_board_get_piece_at_source(game_context.board, "a1xx")
                        && EMPTY == game_board_get_piece_at_source(game_context.board, "b1xx")
                        && EMPTY == game_board_get_piece_at_source(game_context.board, "c1xx")
                        && EMPTY == game_board_get_piece_at_source(game_context.board, "d1xx")
                        ){
                        game_context_pass_to_state_castling_white_left();
                    } else {
                        game_context_pass_to_state_animating(event.data);
                    }
                    parse_line(commands[GO]);
                    command_history_add_command(" ");
                    command_history_add_command(event.data);
                    game_context.board_dirty = true;
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

void game_context_draw_pre() {
    ui_pre_frame_update();
}

static void game_context_build_scene(){
    game_board_draw(game_context.board);
    if(game_options.draw_attacked_positions)
        game_board_attaked_positions_draw(game_context.selector.board_attacked_positions);
    selector_draw(game_context.selector);
}

void game_context_draw() {
    BeginDrawing();
    ClearBackground(LIGHTGRAY);
    DrawTexture(game_context.background, 0, 0, WHITE);
    if(game_context.board_dirty){
        game_board_calculate_attacked_positions();
        game_context.board_dirty = false;
    }
    
    BeginMode3D(game_context.camera_perspective);
    {
        game_context_build_scene();
    }
    EndMode3D();
}

void game_context_draw_post() {
    gui_draw();
    DrawFPS(900, 19);
    EndDrawing();
}

void minimap_draw() {
    BeginMode3D(game_context.camera_top);
    {
        game_board_draw(game_context.board);
        game_board_pieces_draw(game_context.piece_to_move, game_context.piece_to_move_position_actual);
    }
    EndMode3D();
    DrawText("a b c d e f g h", 865, 180, 19, RED);
    int offset = 18;
    DrawText("8", 845, 36, 19, RED);
    DrawText("7", 845, 36 + offset * 1, 20, RED);
    DrawText("6", 845, 36 + offset * 2, 20, RED);
    DrawText("5", 845, 36 + offset * 3, 20, RED);
    DrawText("4", 845, 36 + offset * 4, 20, RED);
    DrawText("3", 845, 36 + offset * 5, 20, RED);
    DrawText("2", 845, 36 + offset * 6, 20, RED);
    DrawText("1", 845, 36 + offset * 7, 20, RED);
}

void record_frame(){
    game_context.image_data = GetScreenData();
    int centisecondsPerFrame = 5, bitDepth = 15;

    msf_gif_frame(&game_context.gif_state, game_context.image_data.data, bitDepth, centisecondsPerFrame, game_context.image_data.width * 4, false); //frame 1
}

void process_state_animating(){
    game_context.piece_to_move_lerp_percentage += 0.01;
    game_context.piece_to_move_position_actual = Vector3Lerp(game_context.piece_to_move_position_actual, game_context.piece_to_move_position_target, game_context.piece_to_move_lerp_percentage);
    if(Vector3Distance(game_context.piece_to_move_position_actual, game_context.piece_to_move_position_target) < 0.1){
            unsigned int piece = game_board_get_piece_at_target(game_context.board, game_context.target_position);
            
            game_context.piece_to_move_position_actual = game_context.piece_to_move_position_target;
            game_board_set_piece_at_target(game_context.board, game_context.target_position, game_context.piece_to_move);
            game_board_set_piece_at_source(game_context.board, game_context.target_position, EMPTY);
            game_context.state = GAME_STATE_PLAYING;


            if(piece == KNG_B){
                game_context.state = GAME_STATE_WON_WHITE;
                recording_system_stop_recording();
            }
            if(piece == KNG_W){
                game_context.state = GAME_STATE_WON_BLACK;
                recording_system_stop_recording();
            }
    }
    game_context_draw_pre();

    ClearBackground(LIGHTGRAY);
    DrawTexture(game_context.background, 0, 0, WHITE);
    
    BeginMode3D(game_context.camera_perspective);
    {
        game_board_draw(game_context.board);
        game_board_pieces_draw(game_context.piece_to_move, game_context.piece_to_move_position_actual);
        selector_draw(game_context.selector);
    }
    EndMode3D();

    minimap_draw();
    game_context_draw_post();
    recording_system_update();
}

void game_context_update()
{
    game_context_event_process();
    
    switch (game_context.state){
        case GAME_STATE_PLAYING:
            selector_update(&game_context.selector, game_context.board, game_context.selector.board_attacked_positions);
            game_context_draw_pre();
            game_context_draw();
            minimap_draw();
            game_context_draw_post();
            break;
        case GAME_STATE_WON_WHITE:
            game_context_draw_pre();
            game_context_draw();

            DrawText("CHECK MATE!", 155, HEIGHT/3 + 5, 100, BLACK);
            DrawText("CHECK MATE!", 150, HEIGHT/3, 100, RED);

            DrawText("WHITE WINS", 255, HEIGHT/3 + 105, 70, BLACK);
            DrawText("WHITE WINS", 250, HEIGHT/3 + 100, 70, WHITE);

            game_context_draw_post();
            break;
        case GAME_STATE_WON_BLACK:
            game_context_draw_pre();
            game_context_draw();

            DrawText("CHECK MATE!", 155, HEIGHT/3 + 5, 100, BLACK);
            DrawText("CHECK MATE!", 150, HEIGHT/3, 100, RED);
            
            DrawText("BLACK WINS", 255, HEIGHT/3 + 105, 70, WHITE);
            DrawText("BLACK WINS", 250, HEIGHT/3 + 100, 70, BLACK);

            game_context_draw_post();
            break;
        case GAME_STATE_ANIMATING:
            process_state_animating();
            break;
        case GAME_STATE_CASTLING_WHITE_RIGHT:
            game_context.piece_to_move_lerp_percentage += 0.01;
            game_context.piece_to_move_position_actual = Vector3Lerp(game_context.piece_to_move_position_actual, game_context.piece_to_move_position_target, game_context.piece_to_move_lerp_percentage);
            game_context.piece_castling_position_actual = Vector3Lerp(game_context.piece_castling_position_actual, game_context.piece_castlint_position_target, game_context.piece_to_move_lerp_percentage);
            if(Vector3Distance(game_context.piece_to_move_position_actual, game_context.piece_to_move_position_target) < 0.1){
                    
                    game_context.piece_to_move_position_actual = game_context.piece_to_move_position_target;
                    game_board_set_piece_at_target(game_context.board, "xxg1", KNG_W);
                    game_board_set_piece_at_source(game_context.board, "e1xx", EMPTY);
                    game_board_set_piece_at_target(game_context.board, "xxf1", TWR_W);
                    game_board_set_piece_at_source(game_context.board, "h1xx", EMPTY);

                    game_context.state = GAME_STATE_PLAYING;
            }
            game_context_draw_pre();

            ClearBackground(LIGHTGRAY);
            DrawTexture(game_context.background, 0, 0, WHITE);
            
            BeginMode3D(game_context.camera_perspective);
            {
                game_board_draw(game_context.board);
                game_board_pieces_draw(KNG_W, game_context.piece_to_move_position_actual);
                game_board_pieces_draw(TWR_W, game_context.piece_castling_position_actual);
            }
            EndMode3D();

            minimap_draw();
            game_context_draw_post();
            recording_system_update();
            break;
        case GAME_STATE_CASTLING_BLACK_RIGHT:
            game_context.piece_to_move_lerp_percentage += 0.01;
            game_context.piece_to_move_position_actual = Vector3Lerp(game_context.piece_to_move_position_actual, game_context.piece_to_move_position_target, game_context.piece_to_move_lerp_percentage);
            game_context.piece_castling_position_actual = Vector3Lerp(game_context.piece_castling_position_actual, game_context.piece_castlint_position_target, game_context.piece_to_move_lerp_percentage);
            if(Vector3Distance(game_context.piece_to_move_position_actual, game_context.piece_to_move_position_target) < 0.1){
                    game_context.piece_to_move_position_actual = game_context.piece_to_move_position_target;
                    game_board_set_piece_at_target(game_context.board, "xxg8", KNG_B);
                    game_board_set_piece_at_source(game_context.board, "e8xx", EMPTY);
                    game_board_set_piece_at_target(game_context.board, "xxf8", TWR_B);
                    game_board_set_piece_at_source(game_context.board, "h8xx", EMPTY);

                    game_context.state = GAME_STATE_PLAYING;
                    game_context.selector.state = SELECTOR_STATE_READY;
            }
            game_context_draw_pre();

            ClearBackground(LIGHTGRAY);
            DrawTexture(game_context.background, 0, 0, WHITE);
            
            BeginMode3D(game_context.camera_perspective);
            {
                game_board_draw(game_context.board);
                game_board_pieces_draw(KNG_B, game_context.piece_to_move_position_actual);
                game_board_pieces_draw(TWR_B, game_context.piece_castling_position_actual);
            }
            EndMode3D();

            minimap_draw();
            game_context_draw_post();
            recording_system_update();
            break;
        case GAME_STATE_CASTLING_WHITE_LEFT:
            game_context.piece_to_move_lerp_percentage += 0.01;
            game_context.piece_to_move_position_actual = Vector3Lerp(game_context.piece_to_move_position_actual, game_context.piece_to_move_position_target, game_context.piece_to_move_lerp_percentage);
            game_context.piece_castling_position_actual = Vector3Lerp(game_context.piece_castling_position_actual, game_context.piece_castlint_position_target, game_context.piece_to_move_lerp_percentage);
            if(Vector3Distance(game_context.piece_to_move_position_actual, game_context.piece_to_move_position_target) < 0.1){
                    
                    game_context.piece_to_move_position_actual = game_context.piece_to_move_position_target;
                    game_board_set_piece_at_target(game_context.board, "xxc1", KNG_W);
                    game_board_set_piece_at_source(game_context.board, "e1xx", EMPTY);
                    game_board_set_piece_at_target(game_context.board, "xxd1", TWR_W);
                    game_board_set_piece_at_source(game_context.board, "a1xx", EMPTY);

                    game_context.state = GAME_STATE_PLAYING;
            }
            game_context_draw_pre();

            ClearBackground(LIGHTGRAY);
            DrawTexture(game_context.background, 0, 0, WHITE);
            
            BeginMode3D(game_context.camera_perspective);
            {
                game_board_draw(game_context.board);
                game_board_pieces_draw(KNG_W, game_context.piece_to_move_position_actual);
                game_board_pieces_draw(TWR_W, game_context.piece_castling_position_actual);
            }
            EndMode3D();

            minimap_draw();
            game_context_draw_post();
            recording_system_update();
            break;
        case GAME_STATE_CASTLING_BLACK_LEFT:
            game_context.piece_to_move_lerp_percentage += 0.01;
            game_context.piece_to_move_position_actual = Vector3Lerp(game_context.piece_to_move_position_actual, game_context.piece_to_move_position_target, game_context.piece_to_move_lerp_percentage);
            game_context.piece_castling_position_actual = Vector3Lerp(game_context.piece_castling_position_actual, game_context.piece_castlint_position_target, game_context.piece_to_move_lerp_percentage);
            if(Vector3Distance(game_context.piece_to_move_position_actual, game_context.piece_to_move_position_target) < 0.1){
                    
                    game_context.piece_to_move_position_actual = game_context.piece_to_move_position_target;
                    game_board_set_piece_at_target(game_context.board, "xxc8", KNG_B);
                    game_board_set_piece_at_source(game_context.board, "e8xx", EMPTY);
                    game_board_set_piece_at_target(game_context.board, "xxd8", TWR_B);
                    game_board_set_piece_at_source(game_context.board, "a8xx", EMPTY);

                    game_context.state = GAME_STATE_PLAYING;
                    game_context.selector.state = SELECTOR_STATE_READY;
            }
            game_context_draw_pre();

            ClearBackground(LIGHTGRAY);
            DrawTexture(game_context.background, 0, 0, WHITE);
            
            BeginMode3D(game_context.camera_perspective);
            {
                game_board_draw(game_context.board);
                game_board_pieces_draw(KNG_B, game_context.piece_to_move_position_actual);
                game_board_pieces_draw(TWR_B, game_context.piece_castling_position_actual);
            }
            EndMode3D();

            minimap_draw();
            game_context_draw_post();
            recording_system_update();
            break;
    }
}
