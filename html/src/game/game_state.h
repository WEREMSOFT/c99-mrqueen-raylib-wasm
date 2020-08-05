#include <raylib.h>
#include "selector.h"

char commands[COMMAND_COUNT][50] = {
    "uci",
    "ucinewgame",
    "isready"
};

const unsigned int base_board[8][8] = {{TWR_B, KGT_B, BSP_B, QEN_B, KNG_B, BSP_B, KGT_B, TWR_B},
                            {PWN_B, PWN_B, PWN_B, PWN_B, PWN_B, PWN_B, PWN_B, PWN_B},
                            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                            {PWN_W, PWN_W, PWN_W, PWN_W, PWN_W, PWN_W, PWN_W, PWN_W},
                            {TWR_W, KGT_W, BSP_W, QEN_W, KNG_W, BSP_W, KGT_W, TWR_W},
                            };

typedef struct game_state_t 
{
    unsigned int board[8][8];
    char textBoxText[1000];
    Camera3D camera;
    selector_t selector;
} game_state_t;
