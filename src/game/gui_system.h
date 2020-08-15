#ifndef __GUI_SYSTEM_H__
#define __GUI_SYSTEM_H__

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <imconfig.h>
#include <cimgui.h>
#include "cimgui_impl_raylib.h"
#include "game_state.h"

ImDrawData *draw_data;

void gui_init(int screen_width, int screen_height){
    // Initialize imgui
    igCreateContext(NULL);
    igStyleColorsDark(NULL);
    // Initialize keyboard and mouse events
    ImGui_ImplRaylib_Init();
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

    igRender();
    draw_data = igGetDrawData();
    raylib_render_cimgui(draw_data);
}


#endif