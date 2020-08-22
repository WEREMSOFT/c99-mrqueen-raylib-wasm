#ifndef __GAME_STATE_H__
#define __GAME_STATE_H__

#include <raylib.h>
#include "types.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

const unsigned int base_board[8][8] = {{TWR_B, KGT_B, BSP_B, QEN_B, KNG_B, BSP_B, KGT_B, TWR_B},
                            {PWN_B, PWN_B, PWN_B, PWN_B, PWN_B, PWN_B, PWN_B, PWN_B},
                            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                            {PWN_W, PWN_W, PWN_W, PWN_W, PWN_W, PWN_W, PWN_W, PWN_W},
                            {TWR_W, KGT_W, BSP_W, QEN_W, KNG_W, BSP_W, KGT_W, TWR_W},
                            };


enum game_state_enum {
    GAME_STATE_PLAYING,
    GAME_STATE_ANIMATING,
    GAME_STATE_WON_WHITE,
    GAME_STATE_WON_BLACK,
};

typedef struct selector_t {
    Vector3 position;
    Vector3 position_start;
    unsigned int state;
    unsigned int origin_piece;
    Color color;
    bool visible;
} selector_t;

typedef struct game_state_t 
{
    int state;
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
} game_state_t;

char commands[COMMAND_COUNT][50] = {
    "uci",
    "ucinewgame",
    "isready",
    "go"
};

#endif