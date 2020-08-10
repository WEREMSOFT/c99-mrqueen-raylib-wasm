#include <time.h>
#include <unistd.h>
#include <stdio.h>

#include "game/console_output.h"
#include "mister_queen/bb.h"
#include "mister_queen/uci.h"
#include "mister_queen/util.h"
#include "game/types.h"
#include "game/game_state.h"
#include "game/selector.h"
#include "game/event.h"
#include "game/command_history.h"

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
char console_buffer[500] = {0};

void console_draw()
{
    static char command[100] = {0};
    
    GuiDrawRectangle((Rectangle){0, 0, 800, 400}, 1, DARKGRAY, LIGHTGRAY);
    GuiTextBoxMulti((Rectangle){10, 10, 780, 300}, console_buffer, 10000, false);
    GuiTextBox((Rectangle){10, 315, 780, 30}, command, 100, true);
    if (GuiButton((Rectangle){10, 355, 125, 30}, GuiIconText(RICON_200, "Send Command")) || IsKeyPressed(KEY_ENTER))
    {
        parse_line(command);
        command[0] = '\0';
    }
}

Camera3D camera_init()
{
    Camera3D return_value = {0};

    return_value.fovy = 45.0f;
    return_value.target = (Vector3){3.5f, 0.0f, 2.f};
    return_value.type = CAMERA_PERSPECTIVE;
    return_value.up = (Vector3){0, 1.0f, 0};
    return_value.position = (Vector3){3.5f, 6.0f, 13.0f};

    return return_value;
}

void game_board_pieces_draw(int piece, Vector3 position)
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

void game_board_draw(game_state_t *game_state)
{
    float offsetX = 0.0f;
    float offsetZ = 0.0f;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Vector3 pos = (Vector3){i + offsetX, -0.5f, j + offsetZ};
            game_board_pieces_draw(game_state->board[j][i], pos);
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
        parse_line(commands[UCI]);
        parse_line(commands[UCINEWGAME]);
        parse_line((char *)&commands[ISREADY]);
        game_board_reset(game_state);
        char *history_buffer = command_history_get_buffer();
        memset(history_buffer, 0, COMMAND_HISTORY_SIZE);
    }
}

void update_frame(void)
{
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
                snprintf(console_buffer, 500, "%s", event.data);
                printf("=> %s\n", event.data);
                break;
            default:
                printf("Unknow event: %s\n", event.data);
        }
        printf("command history: %s\n", command_history_get_buffer());
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

    game_state.camera = camera_init();
    game_state.selector.state = SELECTOR_STATE_READY;
    
    game_board_reset(&game_state);

    InitWindow(WIDTH, HEIGHT, "This is a chess game");
    SetTargetFPS(60);

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
