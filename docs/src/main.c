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

#include "game/game_context.h"

#include "game/selector.h"
#include "game/event.h"
#include "game/game_board.h"
#include "game/gui_system.h"

#include "game/recording_system.h"

#include <raylib.h>
#include <rlights.h>

#ifdef OS_WEB
#include <emscripten/emscripten.h>
#endif

extern game_context_t game_context;
game_options_t game_options = {0};
#if defined(OS_WEB)
#define GLSL_VERSION            100
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            330
#endif

void lights_init(game_context_t* game_state){
    game_state->shader = LoadShader(FormatText("./assets/shaders/glsl%i/base_lighting.vs", GLSL_VERSION),
                                FormatText("./assets/shaders/glsl%i/lighting.fs", GLSL_VERSION));

    game_state->shader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(game_state->shader, "matModel");
    game_state->shader.locs[LOC_VECTOR_VIEW] = GetShaderLocation(game_state->shader, "viewPos");

    int ambientLoc = GetShaderLocation(game_state->shader, "ambient");
    SetShaderValue(game_state->shader, ambientLoc, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, UNIFORM_VEC4);

    game_state->light1 = CreateLight(LIGHT_POINT, (Vector3){ 6.7f, 4, 6.4f }, Vector3Zero(), GRAY, game_state->shader);
    game_state->light2 = CreateLight(LIGHT_POINT, (Vector3){ .2f, 4, 5.1f }, Vector3Zero(), GRAY, game_state->shader);
    game_state->light3 = CreateLight(LIGHT_POINT, (Vector3){ 4.0f, 4, 11.0f }, Vector3Zero(), GRAY, game_state->shader);
    UpdateLightValues(game_state->shader, game_state->light1);
    UpdateLightValues(game_state->shader, game_state->light2);
    UpdateLightValues(game_state->shader, game_state->light3);
}

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
    while (!WindowShouldClose() && !game_options.should_close_window)
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
