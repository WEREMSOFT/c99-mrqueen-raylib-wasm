#include <time.h>
#include <unistd.h>
#include <stdio.h>
#define DEBUG_PRINT
#include "console_output.h"
#include "bb.h"
#include "uci.h"
#include "util.h"

#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include <raygui.h>

#undef RAYGUI_IMPLEMENTATION

#ifdef OS_WEB
#include <emscripten/emscripten.h>
#endif

#define WIDTH 800
#define HEIGHT 600

enum enum_models {
    MODEL_BOARD,
    MODELS_COUNT
};

char textBoxText[1000] = "";

Camera3D camera = {0};
Model models[MODELS_COUNT] = {0};
Texture textures[MODELS_COUNT] = {0};

void console_draw()
{
    static char command[100] = {0};
    char *c = console_buffer_get();
    GuiTextBoxMulti((Rectangle){10, 10, 780, 300}, c, 10000, false);
    GuiTextBox((Rectangle){10, 315, 780, 30}, command, 100, true);
    if (GuiButton((Rectangle){10, 355, 125, 30}, GuiIconText(RICON_NONE, "Send Command")) || IsKeyPressed(KEY_ENTER))
    {
        parse_line(command);
        command[0] = '\0';
    }
}

void board_draw()
{
    BeginMode3D(camera);
    {
        DrawModel(models[MODEL_BOARD], (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
    } EndMode3D();
}

void update_frame(void)
{
    static unsigned int draw_console = true;
    BeginDrawing();
    {
        ClearBackground(WHITE);
        DrawFPS(700, 10);
        {
            board_draw();
            if(IsKeyPressed(KEY_F1)) draw_console = !draw_console;
            if(draw_console){
                console_draw();
            }

        }
    }
    EndDrawing();
}

#define COMMAND_COUNT 5

int main(void)
{
#ifdef OS_Windows_NT
    printf("Windows dettected\n");
#elif defined OS_Linux
    printf("LINUS dettected\n");
#elif defined OS_Darwin
    printf("MacOS dettected\n");
#elif defined OS_WEB
    printf("Browser dettected\n");
#endif

    printf("The number is: %d\n", 10);

#ifdef __EMSCRIPTEN_PTHREADS__
    printf("Pthreads enabled\n");
#else
    printf("NO THREADS!!!\n");
#endif

    camera.fovy = 45.0f;
    camera.target = (Vector3){0.f, 0.f, 0.f};
    camera.type = CAMERA_PERSPECTIVE;
    camera.up = (Vector3){0, 1.0f, 0};
    camera.position = (Vector3){0, 5.0f, -5.0f};

    InitWindow(WIDTH, HEIGHT, "This is a chess game");
    SetTargetFPS(60);

    models[MODEL_BOARD] = LoadModel("assets/board/board.obj");
    textures[MODEL_BOARD] = LoadTexture("assets/board/board.png");
    models[MODEL_BOARD].materials[0].maps[MAP_DIFFUSE].texture = textures[MODEL_BOARD];

    bb_init();
    prng_seed(time(NULL));
    uci_board_reset();

    // char commands[COMMAND_COUNT][1000] = { "uci", "ucinewgame", "isready", "position startpos moves d2d4", "go wtime 300000 btime", "quit"};

    // for(int i = 0; i < COMMAND_COUNT; i++){
    //     parse_line(commands[i]);
    // }

#ifdef OS_WEB
    emscripten_set_main_loop(update_frame, 0, 1);
#else
    while (!WindowShouldClose())
    {
        update_frame();
    }
#endif
    CloseWindow();

    for(int i = 0; i < MODELS_COUNT; i++) 
    {
        UnloadModel(models[i]);
        UnloadTexture(textures[i]);
    }

    return 0;
}
