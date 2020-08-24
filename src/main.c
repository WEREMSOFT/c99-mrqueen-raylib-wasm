#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <raymath.h>
#include "game/console_output.h"
#include "mister_queen/bb.h"
#include "mister_queen/uci.h"
#include "mister_queen/util.h"
#include "game/types.h"

#define WIDTH 1024
#define HEIGHT 800
#include "game/game.h"

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

int main(void)
{
    game_context_t game_context = {0};

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

    selector_pass_to_state_ready(&game_context.selector);

    game_board_reset(game_context.board);

    InitWindow(WIDTH, HEIGHT, "This is a chess game");
    SetTargetFPS(60);

    game_context.background = LoadTexture("assets/background_fight.png");

    game_context.camera_top = camera_top_init();
    game_context.camera_perspective = camera_perspective_init();
    
    gui_init(WIDTH, HEIGHT);
    
    lights_init(&game_context);
    
    game_board_models_load(game_context.shader);

    bb_init();
    prng_seed(time(NULL));
    uci_board_reset();

#ifdef OS_WEB
    emscripten_set_main_loop_arg(game_update, &game_context, 0, 1);
#else
    while (!WindowShouldClose())
    {
        game_update(&game_context);
    }
#endif
    gui_fini();
    command_history_fini();
    game_board_models_unload();
    UnloadTexture(game_context.background);
    CloseWindow();
    return 0;
}
