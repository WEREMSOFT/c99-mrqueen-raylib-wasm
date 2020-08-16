#ifndef __TYPES_H__
#define __TYPES_H__
enum enum_models
{
    MODEL_BOARD,
    MODELS_COUNT
};

enum enum_pieces
{
    EMPTY,
    PWN_B,
    TWR_B,
    BSP_B,
    KGT_B,
    KNG_B,
    QEN_B,
    PWN_W,
    TWR_W,
    BSP_W,
    KGT_W,
    KNG_W,
    QEN_W,
    PIECES_COUNT
};

enum enum_commands {
    UCI,
    UCINEWGAME,
    ISREADY,
    GO,
    COMMAND_COUNT
};
#endif