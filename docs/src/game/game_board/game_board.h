#ifndef __GAME_BOARD_H__
#define __GAME_BOARD_H__
#include <raygui.h>

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
    PIECE_IN_MOTION,
    PIECES_COUNT
};

enum ModelsEnum{
    MODEL_PAWN,
    MODEL_BISHOP,
    MODEL_QUEEN,
    MODEL_KING,
    MODEL_KNIGHT,
    MODEL_ROOK,
    MODEL_COUNT
};

typedef struct game_board_matrix_coordinates_t {
    unsigned int source_x;
    unsigned int source_y;
    unsigned int target_x;
    unsigned int target_y;

} game_board_matrix_coordinates_t;

void game_board_reset(unsigned int board[8][8]);
game_board_matrix_coordinates_t game_board_get_coordinates_in_matrix(char *coords);
Vector3 game_board_get_target_coordinates_in_world(char *coords);
Vector3 game_board_get_source_coordinates_in_world(char *coords);
unsigned int game_board_get_piece_at_target(unsigned int board[8][8], char *coords);
unsigned int game_board_get_piece_at_source(unsigned int board[8][8], char *coords);
void game_board_set_piece_at_source(unsigned int board[8][8], char *coords, unsigned int piece);
void game_board_set_piece_at_target(unsigned int board[8][8], char *coords, unsigned int piece);
void game_board_move_piece(unsigned int board[8][8], char *coords);
void game_board_pieces_draw(int piece, Vector3 position);
void game_board_models_load(Shader shader);
void game_board_models_unload(void);
void game_board_draw(unsigned int board[8][8]);
void game_board_attaked_positions_draw(unsigned int board[8][8]);

#endif