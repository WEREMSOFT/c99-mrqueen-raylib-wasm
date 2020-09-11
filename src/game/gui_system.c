#ifndef __GUI_SYSTEM_H__
#define __GUI_SYSTEM_H__

#include "gui_system.h"
#include "cimgui_impl_raylib.h"
#include <raylib.h>
#include "event.h"
#include "recording_system.h"
#include "game_options.h"
#include "commands.h"
#include "../mister_queen/uci.h"

extern game_options_t game_options;
extern char commands[COMMAND_COUNT][50];
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

void gui_fini(void){
    UnloadImage(image);
    UnloadTexture(texture);
}

void gui_draw(void){
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
                TakeScreenshot("the_only_screenshot.png");
            };
            if(igMenuItemBool(recording_system_is_ready() ? "Start Recording" : "Stop Recording", "", false, true)){
                switch(recording_system_state()){
                    case RECORDING_SYSTEM_STATE_READY:
                        recording_system_start_recording();
                        break;
                    case RECORDING_SYSTEM_STATE_RECORDING:
                        recording_system_stop_recording();
                        break;
                }
            };
            if(igMenuItemBool("Quit", "Esc", false, true)){
                game_options.should_close_window = true;
            };
            igEndMenu();
        }
        if (igBeginMenu("Options", true)) {
            if(igMenuItemBool("FullScreen", "", game_options.is_full_creen, true)){
                game_options.is_full_creen = !game_options.is_full_creen;
                ToggleFullscreen();
            }
            if(igMenuItemBool("Show coordinates on minimap", "", game_options.show_coordinates_on_minimap, true)){
                game_options.show_coordinates_on_minimap = !game_options.show_coordinates_on_minimap;
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
            if(igMenuItemBool("Force computer to move", "", NULL, true)){
                parse_line(commands[GO]);
            }
            igEndMenu();
        }
        igEndMainMenuBar();
    }

    igRender();
    draw_data = igGetDrawData();
    raylib_render_cimgui(draw_data);
}

void ui_pre_frame_update(void){
    ImGui_ImplRaylib_NewFrame();
    ImGui_ImplRaylib_ProcessEvent();
    igNewFrame();
}


#endif