#include "cimgui_impl_raylib.h"

static double g_Time = 0.0;

static const char* ImGui_ImplRaylib_GetClipboardText(void* _)
{
    return GetClipboardText();
}

static void ImGui_ImplRaylib_SetClipboardText(void* _, const char* text)
{
    SetClipboardText(text);
}

bool ImGui_ImplRaylib_Init()
{
    rlEnableScissorTest(); 
    struct ImGuiIO* io = igGetIO();

    io->BackendPlatformName = "cimgui_impl_raylib";

    io->KeyMap[ImGuiKey_Tab] = KEY_TAB;
    io->KeyMap[ImGuiKey_LeftArrow] = KEY_LEFT;
    io->KeyMap[ImGuiKey_RightArrow] = KEY_RIGHT;
    io->KeyMap[ImGuiKey_UpArrow] = KEY_UP;
    io->KeyMap[ImGuiKey_DownArrow] = KEY_DOWN;
    io->KeyMap[ImGuiKey_PageUp] = KEY_PAGE_DOWN;
    io->KeyMap[ImGuiKey_PageDown] = KEY_PAGE_UP;
    io->KeyMap[ImGuiKey_Home] = KEY_HOME;
    io->KeyMap[ImGuiKey_End] = KEY_END;
    io->KeyMap[ImGuiKey_Insert] = KEY_INSERT;
    io->KeyMap[ImGuiKey_Delete] = KEY_DELETE;
    io->KeyMap[ImGuiKey_Backspace] = KEY_BACKSPACE;
    io->KeyMap[ImGuiKey_Space] = KEY_SPACE;
    io->KeyMap[ImGuiKey_Enter] = KEY_ENTER;
    io->KeyMap[ImGuiKey_Escape] = KEY_ESCAPE;
    io->KeyMap[ImGuiKey_KeyPadEnter] = KEY_KP_ENTER;
    io->KeyMap[ImGuiKey_A] = KEY_A;
    io->KeyMap[ImGuiKey_C] = KEY_C;
    io->KeyMap[ImGuiKey_V] = KEY_V;
    io->KeyMap[ImGuiKey_X] = KEY_X;
    io->KeyMap[ImGuiKey_Y] = KEY_Y;
    io->KeyMap[ImGuiKey_Z] = KEY_Z;
    io->KeyMap[ImGuiKeyModFlags_Ctrl] = KEY_LEFT_CONTROL;

    io->MousePos = (ImVec2){-FLT_MAX, -FLT_MAX};
    io->SetClipboardTextFn = ImGui_ImplRaylib_SetClipboardText;
    io->GetClipboardTextFn = ImGui_ImplRaylib_GetClipboardText;
    io->ClipboardUserData = NULL;

    return true;
}

void ImGui_ImplRaylib_Shutdown()
{
    g_Time = 0.0;
}

static void ImGui_ImplRaylib_UpdateMouseCursor()
{
    struct ImGuiIO* io = igGetIO();
    if (io->ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return;

    ImGuiMouseCursor imgui_cursor = igGetMouseCursor();
    if (io->MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        HideCursor();
    }
    else
    {
        // Show OS mouse cursor
        ShowCursor();
    }
}

static void ImGui_ImplRaylib_UpdateMousePosAndButtons()
{
    struct ImGuiIO* io = igGetIO();

    // Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
    if (io->WantSetMousePos)
        SetMousePosition(io->MousePos.x, io->MousePos.y);
    else
        io->MousePos = (ImVec2){-FLT_MAX, -FLT_MAX};

    io->MouseDown[0] = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    io->MouseDown[1] = IsMouseButtonDown(MOUSE_RIGHT_BUTTON);
    io->MouseDown[2] = IsMouseButtonDown(MOUSE_MIDDLE_BUTTON);

    if (!IsWindowMinimized())
        io->MousePos = (ImVec2){GetMouseX(), GetMouseY()};
}

void ImGui_ImplRaylib_NewFrame()
{
    struct ImGuiIO* io = igGetIO();

    io->DisplaySize = (ImVec2){(float) GetScreenWidth(), (float) GetScreenHeight()};

    double current_time = GetTime();
    io->DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f/60.0f);
    g_Time = current_time;

    io->KeyCtrl = IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL);
    io->KeyShift = IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT);
    io->KeyAlt = IsKeyDown(KEY_RIGHT_ALT) || IsKeyDown(KEY_LEFT_ALT);
    io->KeySuper = IsKeyDown(KEY_RIGHT_SUPER) || IsKeyDown(KEY_LEFT_SUPER);

    ImGui_ImplRaylib_UpdateMousePosAndButtons();
    ImGui_ImplRaylib_UpdateMouseCursor();

    if (GetMouseWheelMove() > 0)
        io->MouseWheel += 1;
    else if (GetMouseWheelMove() < 0)
        io->MouseWheel -= 1;
}

bool ImGui_ImplRaylib_ProcessEvent()
{
    struct ImGuiIO* io = igGetIO();

    FOR_ALL_KEYS(SET_KEY_DOWN);

    int keyPressed = GetKeyPressed();
    if (keyPressed > 0)
    {
        printf("processing key event %d\n", keyPressed);
        ImGuiIO_AddInputCharacter(io, keyPressed);
    }
 

    return true;
}

void draw_triangle_vertex(ImDrawVert idx_vert)
{
    Color *c;
    c = (Color *)&idx_vert.col;
    rlColor4ub(c->r, c->g, c->b, c->a);
    rlTexCoord2f(idx_vert.uv.x, idx_vert.uv.y);
    rlVertex2f(idx_vert.pos.x, idx_vert.pos.y);
}

void raylib_render_draw_triangles(unsigned int count, const ImDrawIdx *idx_buffer, const ImDrawVert *idx_vert, unsigned int texture_id)
{
    // Draw the imgui triangle data
    for (unsigned int i = 0; i <= (count - 3); i += 3)
    {
        rlPushMatrix();
        rlBegin(RL_TRIANGLES);
        rlEnableTexture(texture_id);

        ImDrawIdx index;
        ImDrawVert vertex;

        index = idx_buffer[i];
        vertex = idx_vert[index];
        draw_triangle_vertex(vertex);

        index = idx_buffer[i + 2];
        vertex = idx_vert[index];
        draw_triangle_vertex(vertex);

        index = idx_buffer[i + 1];
        vertex = idx_vert[index];
        draw_triangle_vertex(vertex);
        rlDisableTexture();
        rlEnd();
        rlPopMatrix();
    }
}

void raylib_render_cimgui(ImDrawData *draw_data)
{
    rlDisableBackfaceCulling();
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList *cmd_list = draw_data->CmdLists[n];
        const ImDrawVert *vtx_buffer = cmd_list->VtxBuffer.Data; // vertex buffer generated by Dear ImGui
        const ImDrawIdx *idx_buffer = cmd_list->IdxBuffer.Data;  // index buffer generated by Dear ImGui
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd *pcmd = &(cmd_list->CmdBuffer.Data)[cmd_i]; // cmd_list->CmdBuffer->data[cmd_i];
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                ImVec2 pos = draw_data->DisplayPos;
                int rectX = (int)(pcmd->ClipRect.x - pos.x);
                int rectY = (int)(pcmd->ClipRect.y - pos.y);
                int rectW = (int)(pcmd->ClipRect.z - rectX);
                int rectH = (int)(pcmd->ClipRect.w - rectY);
                BeginScissorMode(rectX, rectY, rectW, rectH);
                {
                    unsigned int *ti = pcmd->TextureId;
                    raylib_render_draw_triangles(pcmd->ElemCount, idx_buffer, vtx_buffer, *ti);
                }
            }
            idx_buffer += pcmd->ElemCount;
        }
    }
    EndScissorMode();
    rlEnableBackfaceCulling();
}
