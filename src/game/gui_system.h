#ifndef __GUI_SYSTEM_H__
#define __GUI_SYSTEM_H__

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <imconfig.h>
#include <cimgui.h>
#include "cimgui_impl_raylib.h"
#include "game_state.h"
#include "game_board.h"

ImDrawData *draw_data;
Image image = {0};
Texture2D texture = {0};

void gui_init(int screen_width, int screen_height){

    struct ImGuiContext *ctx;
    struct ImGuiIO *io;
    unsigned char *pixels = NULL;

    // Initialize imgui
    igCreateContext(NULL);
    igStyleColorsDark(NULL);
    // Initialize keyboard and mouse events
    ImGui_ImplRaylib_Init();

    // Create textures

    // Build and load the texture atlas into a texture
    // (In the examples/ app this is usually done within the ImGui_ImplXXX_Init() function from one of the demo Renderer)
    io = igGetIO();

    int width = screen_width;
    int height = screen_height;
    ImFontAtlas_GetTexDataAsRGBA32(io->Fonts, &pixels, &width, &height, NULL);
    // At this point you've got the texture data and you need to upload that your your graphic system:
    // After we have created the texture, store its pointer/identifier (_in whichever format your engine uses_) in 'io.Fonts->TexID'.
    // This will be passed back to your via the renderer. Basically ImTextureID == void*. Read FAQ for details about ImTextureID.
    image = LoadImageEx(pixels, width, height);
    texture = LoadTextureFromImage(image); //MyEngine::CreateTextureFromMemoryPixels(pixels, width, height, TEXTURE_TYPE_RGBA32)
    io->Fonts->TexID = (void *)&texture.id;
}

void gui_fini(){
    UnloadImage(image);
    UnloadTexture(texture);
}

void gui_draw(game_state_t* game_state){
    if (igBeginMainMenuBar())
    {
        if (igBeginMenu("File", true))
        {
            if(igMenuItemBool("New Game", "CTRL+N", false, true)){
                parse_line(commands[UCI]);
                parse_line(commands[UCINEWGAME]);
                parse_line((char *)&commands[ISREADY]);
                game_board_reset(game_state);
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

    igRender();
    draw_data = igGetDrawData();
    raylib_render_cimgui(draw_data);
}

void ui_pre_frame_update(){
    ImGui_ImplRaylib_NewFrame();
    ImGui_ImplRaylib_ProcessEvent();
    igNewFrame();
}


#endif