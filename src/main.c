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

enum enum_models
{
    MODEL_BOARD,
    MODELS_COUNT
};

enum enum_pieces
{
    EMPTY,
    PWN_W,
    TWR_W,
    BSP_W,
    KGT_W,
    KNG_W,
    QEN_W,
    PWN_B,
    TWR_B,
    BSP_B,
    KGT_B,
    KNG_B,
    QEN_B,
    PIECES_COUNT
};

char textBoxText[1000] = "";

unsigned int board[8][8] = {{TWR_B, KGT_B, BSP_B, QEN_B, KNG_B, BSP_B, KGT_B, TWR_B},
                            {PWN_B, PWN_B, PWN_B, PWN_B, PWN_B, PWN_B, PWN_B, PWN_B},
                            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                            {PWN_W, PWN_W, PWN_W, PWN_W, PWN_W, PWN_W, PWN_W, PWN_W},
                            {TWR_W, KGT_W, BSP_W, QEN_W, KNG_W, BSP_W, KGT_W, TWR_W},
                            };

Camera3D camera = {0};

Camera3D init_camera()
{
    Camera3D return_value = {0};

    return_value.fovy = 45.0f;
    return_value.target = (Vector3){-0.5f, 0.0f, 0.f};
    return_value.type = CAMERA_PERSPECTIVE;
    return_value.up = (Vector3){0, 1.0f, 0};
    return_value.position = (Vector3){-0.5f, 4.0f, 9.0f};

    return return_value;
}

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

Vector3 selector_draw()
{
    static Vector3 selector_position = {0};

    if (IsKeyPressed(KEY_LEFT))
    {
        selector_position.x -= 1.0f;
    }

    if (IsKeyPressed(KEY_RIGHT))
    {
        selector_position.x += 1.0f;
    }

    if (IsKeyPressed(KEY_UP))
    {
        selector_position.z -= 1.0f;
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        selector_position.z += 1.0f;
    }

    DrawCubeWires(selector_position, 1, 1, 1, RED);

    return selector_position;
}

void pieces_draw(int x, int y, Vector3 position)
{
    switch (board[y][x])
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

void board_draw()
{
    float offsetX = -4.0f;
    float offsetZ = -4.0f;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Vector3 pos = (Vector3){i + offsetX, -0.5f, j + offsetZ};
            pieces_draw(i, j, pos);
            DrawCube(pos, 1.0f, 0.2f, 1.0f, ((i + j) % 2) ? BLACK : WHITE);
        }
    }
}

void update_frame(void)
{
    BeginDrawing();
    {
        ClearBackground(BLUE);
        {
            static unsigned int draw_console = false;
            BeginMode3D(camera);
            {
                board_draw();
                selector_draw();
            }
            EndMode3D();
            if (IsKeyPressed(KEY_F1))
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

    camera = init_camera();

    InitWindow(WIDTH, HEIGHT, "This is a chess game");
    SetTargetFPS(60);

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
    return 0;
}
