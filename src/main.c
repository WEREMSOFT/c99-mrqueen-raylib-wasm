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


char textBoxText[1000] = "";

void update_frame(void) {
    static char command[100] = {0};
    BeginDrawing();
    {
        ClearBackground(WHITE);
        DrawFPS(10, 10);
        {
            char *c = console_buffer_get();
            GuiTextBoxMulti((Rectangle){ 10, 10, 780, 300 }, c, 10000, false);
            GuiTextBox((Rectangle){ 10, 315, 780, 30 }, command, 100, true);
            if (GuiButton((Rectangle){ 10, 355, 125, 30 }, GuiIconText(RICON_NONE, "Send Command")) 
            || IsKeyPressed(KEY_ENTER))
            {
                parse_line(command);
                command[0] = '\0';
            }
        }
    }EndDrawing();
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

    InitWindow(WIDTH, HEIGHT, "This is a chess game");
    SetTargetFPS(60);

    bb_init();
    prng_seed(time(NULL));
    uci_board_reset();
    
    char commands[COMMAND_COUNT][1000] = { "uci", "ucinewgame", "isready", "position startpos moves d2d4", "go wtime 300000 btime", "quit"};

    for(int i = 0; i < COMMAND_COUNT; i++){
        parse_line(commands[i]);
    }

#ifdef OS_WEB
    emscripten_set_main_loop(update_frame, 0, 1);
#else
    while (!WindowShouldClose())
    {
        update_frame();
    }
#endif
    CloseWindow();

    return 0;
}

