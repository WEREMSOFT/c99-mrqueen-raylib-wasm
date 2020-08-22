#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <raymath.h>
#include "game/console_output.h"
#include "mister_queen/bb.h"
#include "mister_queen/uci.h"
#include "mister_queen/util.h"
#include "game/types.h"
#include "game/game_state.h"
#include "game/selector.h"
#include "game/event.h"
#include "game/command_history.h"
#include "game/game_board.h"
#include "game/gui_system.h"
#include "game/light_system.h"

#include <raylib.h>

#ifdef OS_WEB
#include <emscripten/emscripten.h>
#endif

#define WIDTH 1024
#define HEIGHT 800

game_state_t game_state = {0};

Camera3D camera_perspective_init()
{
    Camera3D return_value = {0};

    return_value.fovy = 45.0f;
    return_value.target = (Vector3){3.5f, 0.0f, 2.f};
    return_value.type = CAMERA_PERSPECTIVE;
    return_value.up = (Vector3){0, 1.0f, 0};
    return_value.position = (Vector3){3.5f, 6.0f, 13.0f};
    return return_value;
}

Camera3D camera_top_init()
{
    Camera3D return_value = {0};

    return_value.fovy = 45.0f;
    return_value.target = (Vector3){-20.f, 0.0f, 20.f};
    return_value.type = CAMERA_ORTHOGRAPHIC;
    return_value.up = (Vector3){0, 0, -1.0f};
    return_value.position = (Vector3){-20.f, 20.0f, 20.0f};
    return return_value;
}

void event_process(){
    event_t event = {0};
    while((event = event_dequeue()).type)
    {
        switch(event.type){
            case EVENT_RESPONSE:
            {
                printf("position sent %s", event.data);

                unsigned int piece = game_board_get_piece_at_target(game_state.board, event.data);

                if(piece == KNG_W){
                    game_state.state = GAME_STATE_WON_BLACK;
                }

                game_board_move_piece(game_state.board, event.data);
                command_history_add_command(" ");
                command_history_add_command(event.data);
                selector_pass_to_state_ready(&game_state.selector);
            }
            break;
            case EVENT_COMMAND:
                {

                    unsigned int piece = game_board_get_piece_at_target(game_state.board, event.data);
                    game_board_move_piece(game_state.board, event.data);

                    if(piece == KNG_B){
                        game_state.state = GAME_STATE_WON_WHITE;
                        break;
                    }
                    char command_as_string[300] = {0};

                    snprintf(command_as_string, 
                        300, 
                        "position startpos moves%s %s",
                        command_history_get_buffer(),
                        event.data);

                    parse_line(command_as_string);
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

void draw_frame_pre() {
    BeginDrawing();
    
    ClearBackground(LIGHTGRAY);
    DrawTexture(game_state.background, 0, 0, WHITE);
    
    BeginMode3D(game_state.camera_perspective);
    {
        game_board_draw(&game_state);
        selector_draw(game_state.selector);
    }
    EndMode3D();

    BeginMode3D(game_state.camera_top);
    {
        game_board_draw(&game_state);
        selector_draw(game_state.selector);
    }
    EndMode3D();

}

void draw_frame_post() {
    gui_draw(&game_state);           
    DrawFPS(900, 19);
    EndDrawing();
}

void update_frame(void)
{
    UpdateCamera(&game_state.camera_perspective);
    ui_pre_frame_update();

    event_process();
    
    switch (game_state.state){
        case GAME_STATE_PLAYING:
            selector_update(&game_state);
            draw_frame_pre();
            draw_frame_post();
            break;
        case GAME_STATE_WON_WHITE:
            draw_frame_pre();

            DrawText("CHECK MATE!", 155, HEIGHT/2 + 5, 100, BLACK);
            DrawText("CHECK MATE!", 150, HEIGHT/2, 100, RED);

            DrawText("WHITE WINS", 255, HEIGHT/2 + 105, 70, BLACK);
            DrawText("WHITE WINS", 250, HEIGHT/2 + 100, 70, WHITE);

            draw_frame_post();
            break;
        case GAME_STATE_WON_BLACK:
            draw_frame_pre();

            DrawText("CHECK MATE!", 155, HEIGHT/2 + 5, 100, BLACK);
            DrawText("CHECK MATE!", 150, HEIGHT/2, 100, RED);
            
            DrawText("BLACK WINS", 255, HEIGHT/2 + 105, 70, WHITE);
            DrawText("BLACK WINS", 250, HEIGHT/2 + 100, 70, BLACK);

            draw_frame_post();
            break;
    }

}

int main(void)
{
    command_history_init();

#ifdef OS_Windows_NT
    simple_printf("Windows dettected\n");
#elif defined OS_Linux
    simple_printf("LINUS dettected\n");
#elif defined OS_Darwin
    simple_printf("MacOS dettected\n");
#elif defined OS_WEB
    simple_printf("Browser dettected\n");
#endif

    selector_pass_to_state_ready(&game_state.selector);

    game_board_reset(&game_state);

    InitWindow(WIDTH, HEIGHT, "This is a chess game");
    SetTargetFPS(60);

    game_state.background = LoadTexture("assets/background_fight.png");

    game_state.camera_top = camera_top_init();
    game_state.camera_perspective = camera_perspective_init();
    
    gui_init(WIDTH, HEIGHT);
    
    lights_init(&game_state);
    
    game_board_models_load(&game_state);

    bb_init();
    prng_seed(time(NULL));
    uci_board_reset();

#ifdef OS_WEB
    emscripten_set_main_loop(update_frame, 0, 1);
#else
    while (!WindowShouldClose())
    {
        update_frame();
    }
#endif
    gui_fini();
    command_history_fini();
    UnloadTexture(game_state.background);
    CloseWindow();
    return 0;
}
