#include <time.h>
#include <unistd.h>
#include <stdio.h>
#define DEBUG_PRINT
#include "game/console_output.h"
#include "mister_queen/bb.h"
#include "mister_queen/uci.h"
#include "mister_queen/util.h"
#include "game/types.h"
#include "game/game_state.h"

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

game_state_t game_state = {0};

Camera3D camera_init()
{
    Camera3D return_value = {0};

    return_value.fovy = 45.0f;
    return_value.target = (Vector3){-0.5f, 0.0f, 0.f};
    return_value.type = CAMERA_PERSPECTIVE;
    return_value.up = (Vector3){0, 1.0f, 0};
    return_value.position = (Vector3){-0.5f, 7.0f, 7.0f};

    return return_value;
}

void console_draw()
{
    static char command[100] = {0};
    char *c = console_buffer_get();
    int pending_commands = console_buffer_pending_commands();
    if(pending_commands > 0)
    {
        printf("Pending commands %d\n", pending_commands);
    }
    GuiDrawRectangle((Rectangle){0, 0, 800, 400}, 1, DARKGRAY, LIGHTGRAY);
    GuiTextBoxMulti((Rectangle){10, 10, 780, 300}, c, 10000, false);
    GuiTextBox((Rectangle){10, 315, 780, 30}, command, 100, true);
    if (GuiButton((Rectangle){10, 355, 125, 30}, GuiIconText(RICON_NONE, "Send Command")) || IsKeyPressed(KEY_ENTER))
    {
        parse_line(command);
        command[0] = '\0';
    }
    console_buffer_clear_pending_commands();
}

void pieces_draw(int piece, Vector3 position)
{
    switch (piece)
    {
        case PWN_B:
            DrawCylinder(position, 0.2f, 0.4f, 1.0f, 10, BLACK);
            DrawCylinderWires(position, 0.2f, 0.4f, 1.0f, 10, LIGHTGRAY);
            break;
        case PWN_W:
            DrawCylinder(position, 0.2f, 0.4f, 1.0f, 10, LIGHTGRAY);
            DrawCylinderWires(position, 0.2f, 0.4f, 1.0f, 10, BLACK);
            break;
        case TWR_W:
            DrawCylinder(position, 0.6f, 0.4f, 1.5f, 4, LIGHTGRAY);
            DrawCylinderWires(position, 0.6f, 0.4f, 1.5f, 4, BLACK);
            break;
        case TWR_B:
            DrawCylinder(position, 0.6f, 0.4f, 1.5f, 4, BLACK);
            DrawCylinderWires(position, 0.6f, 0.4f, 1.5f, 4, LIGHTGRAY);
            break;
        case BSP_B:
            DrawCylinder(position, 0.2f, 0.4f, 1.5f, 10, BLACK);
            DrawCylinderWires(position, 0.2f, 0.4f, 1.5f, 10, LIGHTGRAY);
            break;
        case BSP_W:
            DrawCylinder(position, 0.2f, 0.4f, 1.5f, 10, LIGHTGRAY);
            DrawCylinderWires(position, 0.2f, 0.4f, 1.5f, 10, BLACK);
            break;
        case KGT_W:
            DrawCylinder(position, 0.2f, 0.4f, 1.5f, 4, LIGHTGRAY);
            DrawCylinderWires(position, 0.2f, 0.4f, 1.5f, 4, BLACK);
            break;
        case KGT_B:
            DrawCylinder(position, 0.2f, 0.4f, 1.5f, 4, BLACK);
            DrawCylinderWires(position, 0.2f, 0.4f, 1.5f, 4, LIGHTGRAY);
            break;
        case KNG_B:
            DrawCylinder(position, .0f, 0.4f, 2.0f, 4, BLACK);
            DrawCylinderWires(position, .0f, 0.4f, 2.0f, 4, LIGHTGRAY);
            break;
        case KNG_W:
            DrawCylinder(position, .0f, 0.4f, 2.0f, 4, LIGHTGRAY);
            DrawCylinderWires(position, .0f, 0.4f, 2.0f, 4, BLACK);
            break;
        case QEN_B:
            DrawCylinder(position, .0f, 0.4f, 2.0f, 10, BLACK);
            DrawCylinderWires(position, .0f, 0.4f, 2.0f, 10, LIGHTGRAY);
            break;
        case QEN_W:
            DrawCylinder(position, .0f, 0.4f, 2.0f, 10, LIGHTGRAY);
            DrawCylinderWires(position, .0f, 0.4f, 2.0f, 10, BLACK);
            break;
        default:
            break;
    }
}

void board_draw(game_state_t *game_state)
{
    float offsetX = -4.0f;
    float offsetZ = -4.0f;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Vector3 pos = (Vector3){i + offsetX, -0.5f, j + offsetZ};
            pieces_draw(game_state->board[j][i], pos);
            DrawCube(pos, 1.0f, 0.2f, 1.0f, ((i + j) % 2) ? BLACK : WHITE);
        }
    }
}

void game_board_reset(game_state_t* game_state){
    memcpy(game_state->board, base_board, sizeof(game_state->board));
}

void in_game_ui_draw(game_state_t* game_state){
    if (GuiButton((Rectangle){10, 10, 125, 30}, GuiIconText(RICON_NONE, "New Game")))
    {
        parse_line(&commands[UCI]);
        parse_line(&commands[UCINEWGAME]);
        parse_line(&commands[ISREADY]);
        game_board_reset(game_state);
    }
}

void update_frame(void)
{
    BeginDrawing();
    {
        ClearBackground(BLUE);
        {
            static unsigned int draw_console = false;
            BeginMode3D(game_state.camera);
            {
                board_draw(&game_state);
                selector_draw(&game_state.selector);
            }
            EndMode3D();
            in_game_ui_draw(&game_state);
            if (IsKeyPressed(KEY_TAB))
                draw_console = !draw_console;
            if (draw_console)
            {
                console_draw();
            }
        }
        DrawFPS(700, 10);
    }
    EndDrawing();
}

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

    game_state.camera = camera_init();
    game_state.selector.state = SELECTOR_STATE_READY;
    
    game_board_reset(&game_state);

    InitWindow(WIDTH, HEIGHT, "This is a chess game");
    SetTargetFPS(60);

    bb_init();
    prng_seed(time(NULL));
    uci_board_reset();

    // char commands[COMMAND_COUNT][1000] = { "uci", "ucinewgame", "isready", "position startpos moves d2d4", "go wtime 300000 btime", "quit"};
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
