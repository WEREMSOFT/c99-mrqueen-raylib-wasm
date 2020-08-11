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

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include <raygui.h>
#undef RAYGUI_IMPLEMENTATION

#ifdef OS_WEB
#include <emscripten/emscripten.h>
#endif

#if defined(OS_WEB)
#define GLSL_VERSION            100
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            330
#endif

#define WIDTH 800
#define HEIGHT 600

game_state_t game_state = {0};
char console_buffer[500] = {0};

Model model_bishop = {0};
Model model_rook = {0};
Model model_pawn = {0};
Model model_knight = {0};
Model model_king = {0};
Model model_queen = {0};


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
    SetCameraMode(return_value, CAMERA_FREE);
    return return_value;
}

void game_board_pieces_draw(int piece, Vector3 position)
{
    switch (piece)
    {
        case PWN_B:
            DrawModel(model_pawn, position, 0.015f, DARKGRAY);
            break;
        case PWN_W:
            DrawModel(model_pawn, position, 0.015f, WHITE);
            break;
        case TWR_W:
            DrawModel(model_rook, position, 0.015f, WHITE);
            break;
        case TWR_B:
            DrawModel(model_rook, position, 0.015f, DARKGRAY);
            break;
        case BSP_B:
            DrawModel(model_bishop, position, 0.015f, DARKGRAY);
            break;
        case BSP_W:
            DrawModel(model_bishop, position, 0.015f, WHITE);
            break;
        case KGT_W:
            DrawModel(model_knight, position, 0.015f, WHITE);
            break;
        case KGT_B:
            DrawModel(model_knight, position, 0.015f, DARKGRAY);
            break;
        case KNG_B:
            DrawModel(model_king, position, 0.015f, DARKGRAY);
            break;
        case KNG_W:
            DrawModel(model_king, position, 0.015f, WHITE);
            break;
        case QEN_B:
            DrawModel(model_queen, position, 0.015f, DARKGRAY);
            break;
        case QEN_W:
            DrawModel(model_queen, position, 0.015f, WHITE);
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
            DrawCube(pos, 1.0f, 0.2f, 1.0f, ((i + j) % 2) ? DARKGRAY : WHITE);
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
    UpdateCamera(&game_state.camera);
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

Shader shader = {0};

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


    shader = LoadShader(FormatText("./assets/shaders/glsl%i/base_lighting.vs", GLSL_VERSION),
                               FormatText("./assets/shaders/glsl%i/lighting.fs", GLSL_VERSION));

    shader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, UNIFORM_VEC4);

    Light light_1 = CreateLight(LIGHT_POINT, (Vector3){ 0, 4, 4 }, Vector3Zero(), GRAY, shader);
    Light light_2 = CreateLight(LIGHT_POINT, (Vector3){ 8, 15, 4 }, Vector3Zero(), GRAY, shader);
    // Light light_3 = CreateLight(LIGHT_POINT, (Vector3){ 5, 5, 5 }, Vector3Zero(), PURPLE, shader);
    UpdateLightValues(shader, light_1);
    UpdateLightValues(shader, light_2);
    // UpdateLightValues(shader, light_3);
    
    model_bishop = LoadModel("assets/bishop.obj");
    model_bishop.materials[0].shader = shader;

    model_rook = LoadModel("assets/rook.obj");
    model_rook.materials[0].shader = shader;

    model_pawn = LoadModel("assets/pawn.obj");
    model_pawn.materials[0].shader = shader;

    model_king = LoadModel("assets/king.obj");
    model_king.materials[0].shader = shader;

    model_queen = LoadModel("assets/queen.obj");
    model_queen.materials[0].shader = shader;

    model_knight = LoadModel("assets/knight.obj");
    model_knight.materials[0].shader = shader;

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
