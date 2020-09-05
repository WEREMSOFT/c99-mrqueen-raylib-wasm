#ifndef __GUI_SYSTEM_H__
#define __GUI_SYSTEM_H__

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <imconfig.h>
#include <cimgui.h>
#define MSF_GIF_IMPL
#include <msf_gif.h>
#include "cimgui_impl_raylib.h"
#include "game_options.h"
#include "game_board.h"

ImDrawData *draw_data;
Image image = {0};
Texture2D texture = {0};

void gui_init(int screen_width, int screen_height){

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
    image = LoadImageEx((Color *)pixels, width, height);
    texture = LoadTextureFromImage(image); //MyEngine::CreateTextureFromMemoryPixels(pixels, width, height, TEXTURE_TYPE_RGBA32)
    io->Fonts->TexID = (void *)&texture.id;
}

void gui_fini(){
    UnloadImage(image);
    UnloadTexture(texture);
}

void gui_draw(){
    if (igBeginMainMenuBar())
    {
        if (igBeginMenu("File", true))
        {
            if(igMenuItemBool("New Game", "CTRL+N", false, true)){
                event_t event = {0};
                event.type = EVENT_UI_NEW_GAME;
                event_queue_enqueue(event);
            };
            if(igMenuItemBool("Take Screenshot", "", false, true)){
                Image img = GetScreenData();
                int centisecondsPerFrame = 5, bitDepth = 15;
                MsfGifState gifState = { 0 };
                msf_gif_begin(&gifState, "example.gif", img.width, img.height);
                msf_gif_frame(&gifState, img.data, bitDepth, centisecondsPerFrame, img.width * 4, false); //frame 1
                msf_gif_end(&gifState);
            };

            if(igMenuItemBool("Quit", "Esc", false, true)){
                CloseWindow();
            };
            igEndMenu();
        }
        if (igBeginMenu("Options", true)) {
            if(igMenuItemBool("FullScreen", "", game_options.is_full_creen, true)){
                game_options.is_full_creen = !game_options.is_full_creen;
                ToggleFullscreen();
            }
            if(igMenuItemBool("Show attacked positions", "", game_options.draw_attacked_positions, true)){
                game_options.draw_attacked_positions = !game_options.draw_attacked_positions;
            }
            if(igMenuItemBool("Allow move is king is pinned", "", game_options.allow_move_if_king_is_pinned, true)){
                game_options.allow_move_if_king_is_pinned = !game_options.allow_move_if_king_is_pinned;
            }
            if(igMenuItemBool("Allow move is king if target is under attack", "", game_options.allow_move_king_if_target_is_under_attack, true)){
                game_options.allow_move_king_if_target_is_under_attack = !game_options.allow_move_king_if_target_is_under_attack;
            }
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