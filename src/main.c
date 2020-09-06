#define MSF_GIF_IMPL

#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <raymath.h>
#include "game/console_output.h"
#include "mister_queen/bb.h"
#include "mister_queen/uci.h"
#include "mister_queen/util.h"
#include "game/commands.h"

#define WIDTH 1024
#define HEIGHT 800
#include "game/game_context.h"

#include "game/selector.h"
#include "game/event.h"
#include "game/command_history.h"
#include "game/game_board.h"
#include "game/gui_system.h"
#include "game/light_system.h"

#include "game/recording_system.h"

#include <raylib.h>

#ifdef OS_WEB
#include <emscripten/emscripten.h>
#endif

int main(void)
{
#ifdef OS_Windows_NT
    simple_printf("Windows dettected\n");
#elif defined OS_Linux
    simple_printf("LINUS dettected\n");
#elif defined OS_Darwin
    simple_printf("MacOS dettected\n");
#elif defined OS_WEB
    simple_printf("Browser dettected\n");
#endif
    InitWindow(WIDTH, HEIGHT, "This is a chess game");
    SetTargetFPS(60);
    
    game_context = game_context_init();
    recording_system_init();

    command_history_init();
    selector_pass_to_state_ready(&game_context.selector);
    game_board_reset(game_context.board);
    gui_init(WIDTH, HEIGHT);
    
    lights_init(&game_context);
    
    game_board_models_load(game_context.shader);

    bb_init();
    prng_seed(time(NULL));
    uci_board_reset();
    
#ifdef OS_WEB
    emscripten_set_main_loop_arg(game_context_update, &game_context, 0, 1);
#else
    while (!WindowShouldClose())
    {
        game_context_update(&game_context);
    }
#endif
    gui_fini();
    command_history_fini();
    game_board_models_unload();
    game_context_fini();
    recording_system_fini();
    CloseWindow();
    return 0;
}
