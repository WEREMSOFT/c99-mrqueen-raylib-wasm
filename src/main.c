#define MSF_GIF_IMPL

#include <unistd.h>
#include <stdio.h>
#include "game/console_output.h"
#include "mister_queen/bb.h"
#include "mister_queen/util.h"
#include "game/game_context.h"
#include "game/gui_system.h"
#include <raylib.h>

#ifdef OS_WEB
#include <emscripten/emscripten.h>
#endif

extern game_context_t game_context;
game_options_t game_options = {0};

#ifdef OS_WEB
void web_main_loop(void){
    game_context.update();
}
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
    
    game_board_models_load(game_context.shader);

    bb_init();
    prng_seed(time(NULL));
    uci_board_reset();
    
#ifdef OS_WEB
    emscripten_set_main_loop(web_main_loop, 0, 1);
#else
    while (!WindowShouldClose() && !game_options.should_close_window)
    {
        game_context.update();
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
