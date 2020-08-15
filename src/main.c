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

#include <raylib.h>

#ifdef OS_WEB
#include <emscripten/emscripten.h>
#endif

#if defined(OS_WEB)
#define GLSL_VERSION            100
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            330
#endif

#define WIDTH 1024
#define HEIGHT 800

game_state_t game_state = {0};

Camera3D camera_init()
{
    Camera3D return_value = {0};

    return_value.fovy = 45.0f;
    return_value.target = (Vector3){3.5f, 0.0f, 2.f};
    return_value.type = CAMERA_PERSPECTIVE;
    return_value.up = (Vector3){0, 1.0f, 0};
    return_value.position = (Vector3){3.5f, 6.0f, 13.0f};
    SetCameraMode(return_value, CAMERA_FREE);
    return return_value;
}

void game_board_reset(game_state_t* game_state){
    memcpy(game_state->board, base_board, sizeof(game_state->board));
}

void update_frame(void)
{
    UpdateCamera(&game_state.camera);
    ImGui_ImplRaylib_NewFrame();
    ImGui_ImplRaylib_ProcessEvent();
    igNewFrame();

    event_t event = {0};
    while((event = event_dequeue()).type)
    {
        switch(event.type){
            case EVENT_RESPONSE:
                printf("position sent %s", event.data);
                move_piece(event.data, &game_state);
                command_history_add_command(" ");
                command_history_add_command(event.data);
            break;
            case EVENT_COMMAND:
                move_piece(event.data, &game_state);
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
                break;
            case EVENT_LOG:
                printf("info: %s", event.data);
                break;
            default:
                printf("Unknow event: %s\n", event.data);
        }
    }
    
    BeginDrawing();
    {
        ClearBackground(BLUE);
        {
            static unsigned int draw_console = false;
            BeginMode3D(game_state.camera);
            {
                game_board_draw(&game_state);
                selector_draw(&game_state);
            }
            EndMode3D();
            gui_draw(&game_state);           
        }
        DrawFPS(700, 10);
    }
    EndDrawing();
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

    game_state.selector.state = SELECTOR_STATE_READY;
    
    game_board_reset(&game_state);

    InitWindow(WIDTH, HEIGHT, "This is a chess game");
    SetTargetFPS(60);
    game_state.camera = camera_init();

    // #####################################################
    // cimgui variables
    struct ImGuiContext *ctx;
    struct ImGuiIO *io;

    gui_init(WIDTH, HEIGHT);

    // Create textures

    // Build and load the texture atlas into a texture
    // (In the examples/ app this is usually done within the ImGui_ImplXXX_Init() function from one of the demo Renderer)
    io = igGetIO();
    unsigned char *pixels = NULL;

    int width = WIDTH;
    int height = HEIGHT;
    ImFontAtlas_GetTexDataAsRGBA32(io->Fonts, &pixels, &width, &height, NULL);
    // At this point you've got the texture data and you need to upload that your your graphic system:
    // After we have created the texture, store its pointer/identifier (_in whichever format your engine uses_) in 'io.Fonts->TexID'.
    // This will be passed back to your via the renderer. Basically ImTextureID == void*. Read FAQ for details about ImTextureID.
    Image image = LoadImageEx(pixels, width, height);
    Texture2D texture = LoadTextureFromImage(image); //MyEngine::CreateTextureFromMemoryPixels(pixels, width, height, TEXTURE_TYPE_RGBA32)
    io->Fonts->TexID = (void *)&texture.id;

    // #####################################################

    game_state.shader = LoadShader(FormatText("./assets/shaders/glsl%i/base_lighting.vs", GLSL_VERSION),
                               FormatText("./assets/shaders/glsl%i/lighting.fs", GLSL_VERSION));

    game_state.shader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(game_state.shader, "matModel");
    game_state.shader.locs[LOC_VECTOR_VIEW] = GetShaderLocation(game_state.shader, "viewPos");

    int ambientLoc = GetShaderLocation(game_state.shader, "ambient");
    SetShaderValue(game_state.shader, ambientLoc, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, UNIFORM_VEC4);

    game_state.light1 = CreateLight(LIGHT_POINT, (Vector3){ 6.7f, 4, 6.4f }, Vector3Zero(), GRAY, game_state.shader);
    game_state.light2 = CreateLight(LIGHT_POINT, (Vector3){ .2f, 4, 5.1f }, Vector3Zero(), GRAY, game_state.shader);
    game_state.light3 = CreateLight(LIGHT_POINT, (Vector3){ 4.0f, 4, 11.0f }, Vector3Zero(), GRAY, game_state.shader);
    UpdateLightValues(game_state.shader, game_state.light1);
    UpdateLightValues(game_state.shader, game_state.light2);
    UpdateLightValues(game_state.shader, game_state.light3);
    
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
    CloseWindow();
    command_history_fini();
    return 0;
}
