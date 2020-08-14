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

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <imconfig.h>
#include <cimgui.h>
#include "cimgui_impl_raylib.h"

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

#define WIDTH 1024
#define HEIGHT 800

game_state_t game_state = {0};
char console_buffer[500] = {0};

Model model_bishop = {0};
Model model_rook = {0};
Model model_pawn = {0};
Model model_knight = {0};
Model model_king = {0};
Model model_queen = {0};

ImDrawData *draw_data;

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
    Matrix rotation_matrix = MatrixRotateY(PI);
    Matrix original_matrix = MatrixRotateY(0);

    switch (piece)
    {
        case PWN_B:
            model_pawn.transform = original_matrix;
            DrawModel(model_pawn, position, 0.015f, BLACK);
            break;
        case PWN_W:
            model_pawn.transform = rotation_matrix;
            DrawModel(model_pawn, position, 0.015f, WHITE);
            break;
        case TWR_W:
            model_rook.transform = rotation_matrix;
            DrawModel(model_rook, position, 0.015f, WHITE);
            break;
        case TWR_B:
            model_rook.transform = original_matrix;
            DrawModel(model_rook, position, 0.015f, BLACK);
            break;
        case BSP_B:
            model_bishop.transform = original_matrix;
            DrawModel(model_bishop, position, 0.015f, BLACK);
            break;
        case BSP_W:
            model_bishop.transform = rotation_matrix;
            DrawModel(model_bishop, position, 0.015f, WHITE);
            break;
        case KGT_W:
            model_knight.transform = rotation_matrix;
            DrawModel(model_knight, position, 0.015f, WHITE);
            break;
        case KGT_B:
            model_knight.transform = original_matrix;
            DrawModel(model_knight, position, 0.015f, BLACK);
            break;
        case KNG_B:
            model_king.transform = original_matrix;
            DrawModel(model_king, position, 0.015f, BLACK);
            break;
        case KNG_W:
            model_king.transform = rotation_matrix;
            DrawModel(model_king, position, 0.015f, WHITE);
            break;
        case QEN_B:
            model_queen.transform = original_matrix;
            DrawModel(model_queen, position, 0.015f, BLACK);
            break;
        case QEN_W:
            model_queen.transform = rotation_matrix;
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
                snprintf(console_buffer, 500, "%s", event.data);
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
            //in_game_ui_draw(&game_state);

            //################################################


            if (igBeginMainMenuBar())
            {
                if (igBeginMenu("File", true))
                {
                    if(igMenuItemBool("New Game", "CTRL+N", false, true)){
                        parse_line(commands[UCI]);
                        parse_line(commands[UCINEWGAME]);
                        parse_line((char *)&commands[ISREADY]);
                        game_board_reset(&game_state);
                        char *history_buffer = command_history_get_buffer();
                        memset(history_buffer, 0, COMMAND_HISTORY_SIZE);
                    };
                    if(igMenuItemBool("Quit", "CTRL+X", false, true)){
                        CloseWindow();
                    };
                    igEndMenu();
                }
                if (igBeginMenu("Edit", true))
                {
                    if (igMenuItemBool("Undo", "CTRL+Z", false, true)) {}
                    if (igMenuItemBool("Redo", "CTRL+Y", false, true)) {}  // Disabled item
                    igSeparator();
                    if (igMenuItemBool("Cut", "CTRL+X", false, true)) {}
                    if (igMenuItemBool("Copy", "CTRL+C", false, true)) {}
                    if (igMenuItemBool("Paste", "CTRL+V", false, true)) {}
                    igEndMenu();
                }
                igEndMainMenuBar();
            }

            //###############################################



            
            igRender();
            draw_data = igGetDrawData();
            raylib_render_cimgui(draw_data);

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

    game_state.selector.state = SELECTOR_STATE_READY;
    
    game_board_reset(&game_state);

    InitWindow(WIDTH, HEIGHT, "This is a chess game");
    SetTargetFPS(60);
    game_state.camera = camera_init();


// cimgui variables
    struct ImGuiContext *ctx;
    struct ImGuiIO *io;

    // Initialize imgui
    igCreateContext(NULL);
    igStyleColorsDark(NULL);
    // Initialize keyboard and mouse events
    ImGui_ImplRaylib_Init();

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
    
    model_bishop = LoadModel("assets/bishop.obj");
    model_bishop.materials[0].shader = game_state.shader;

    model_rook = LoadModel("assets/rook.obj");
    model_rook.materials[0].shader = game_state.shader;

    model_pawn = LoadModel("assets/pawn.obj");
    model_pawn.materials[0].shader = game_state.shader;

    model_king = LoadModel("assets/king.obj");
    model_king.materials[0].shader = game_state.shader;

    model_queen = LoadModel("assets/queen.obj");
    model_queen.materials[0].shader = game_state.shader;

    model_knight = LoadModel("assets/knight.obj");
    model_knight.materials[0].shader = game_state.shader;

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
