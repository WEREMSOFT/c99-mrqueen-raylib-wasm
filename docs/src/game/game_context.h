#ifndef __GAME_STATE_H__
#define __GAME_STATE_H__

#include "command_history.h"
#include "selector.h"
#include "../mister_queen/uci.h"
#include <msf_gif.h>
#include <raylib.h>
#include <rlights.h>

#define WIDTH 1024
#define HEIGHT 800

struct game_context_t 
{
    unsigned int board[8][8];
    char textBoxText[1000];
    Camera3D camera_perspective;
    Camera3D camera_top;
    Light light1;
    Light light2;
    Light light3;
    Shader shader;
    selector_t selector;
    Texture2D background;
    char target_position[5];
    Vector3 piece_to_move_position_actual;
    Vector3 piece_to_move_position_target;
    Vector3 piece_castling_position_actual;
    Vector3 piece_castlint_position_target;
    float piece_to_move_lerp_percentage;
    unsigned int piece_to_move;
    bool board_dirty;
    MsfGifState gif_state;
    Image image_data;
    void (*update)(void);
};

typedef struct game_context_t game_context_t;

void calculate_diagonals_to_infinite(int i, int j);
void calculate_square_to_infinite(int i, int j);
void game_board_calculate_attacked_positions(void);
game_context_t game_context_init(void);
void game_context_fini(void);
void game_context_event_process(void);
void game_context_draw_pre(void);
void game_context_draw(void);
void game_context_draw_post(void);
void minimap_draw(void);
void record_frame(void);

#endif