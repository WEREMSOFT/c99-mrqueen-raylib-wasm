#ifndef __GAME_BOARD_H__
#define __GAME_BOARD_H__
#include <stdbool.h>
#include <raygui.h>
#include "game_state.h"

static char board_coord_x[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
static char board_coord_y[8] = {'8', '7', '6', '5', '4', '3', '2', '1'};

enum ModelsEnum{
    MODEL_PAWN,
    MODEL_BISHOP,
    MODEL_QUEEN,
    MODEL_KING,
    MODEL_KNIGHT,
    MODEL_ROOK,
    MODEL_COUNT
};

Model models[MODEL_COUNT] = {0};

bool game_board_is_position_legal(unsigned int board[8][8], selector_t selector) {
    unsigned int piece = board[(int)(selector.position.z)][(int)(selector.position.x)];
    return  piece > 6;
}

void game_board_move_piece(unsigned int board[8][8], char *coords)
{
   unsigned int board_index_cource_x = (unsigned int)(strchr(board_coord_x, coords[0]) - board_coord_x);
   unsigned int board_index_cource_y = (unsigned int)(strchr(board_coord_y, coords[1]) - board_coord_y);
   unsigned int board_index_target_x = (unsigned int)(strchr(board_coord_x, coords[2]) - board_coord_x);
   unsigned int board_index_target_y = (unsigned int)(strchr(board_coord_y, coords[3]) - board_coord_y);

   board[board_index_target_y][board_index_target_x] = board[board_index_cource_y][board_index_cource_x];
   board[board_index_cource_y][board_index_cource_x] = EMPTY;
}

void game_board_reset(game_state_t* game_state){
    memcpy(game_state->board, base_board, sizeof(game_state->board));
}

void game_board_pieces_draw(int piece, Vector3 position)
{
    Matrix rotation_matrix = MatrixRotateY(PI);
    Matrix original_matrix = MatrixRotateY(0);

    switch (piece)
    {
        case PWN_B:
            models[MODEL_PAWN].transform = original_matrix;
            DrawModel(models[MODEL_PAWN], position, 0.015f, BLACK);
            break;
        case PWN_W:
            models[MODEL_PAWN].transform = rotation_matrix;
            DrawModel(models[MODEL_PAWN], position, 0.015f, WHITE);
            break;
        case TWR_W:
            models[MODEL_ROOK].transform = rotation_matrix;
            DrawModel(models[MODEL_ROOK], position, 0.015f, WHITE);
            break;
        case TWR_B:
            models[MODEL_ROOK].transform = original_matrix;
            DrawModel(models[MODEL_ROOK], position, 0.015f, BLACK);
            break;
        case BSP_B:
            models[MODEL_BISHOP].transform = original_matrix;
            DrawModel(models[MODEL_BISHOP], position, 0.015f, BLACK);
            break;
        case BSP_W:
            models[MODEL_BISHOP].transform = rotation_matrix;
            DrawModel(models[MODEL_BISHOP], position, 0.015f, WHITE);
            break;
        case KGT_W:
            models[MODEL_KNIGHT].transform = rotation_matrix;
            DrawModel(models[MODEL_KNIGHT], position, 0.015f, WHITE);
            break;
        case KGT_B:
            models[MODEL_KNIGHT].transform = original_matrix;
            DrawModel(models[MODEL_KNIGHT], position, 0.015f, BLACK);
            break;
        case KNG_B:
            models[MODEL_KING].transform = original_matrix;
            DrawModel(models[MODEL_KING], position, 0.015f, BLACK);
            break;
        case KNG_W:
            models[MODEL_KING].transform = rotation_matrix;
            DrawModel(models[MODEL_KING], position, 0.015f, WHITE);
            break;
        case QEN_B:
            models[MODEL_QUEEN].transform = original_matrix;
            DrawModel(models[MODEL_QUEEN], position, 0.015f, BLACK);
            break;
        case QEN_W:
            models[MODEL_QUEEN].transform = rotation_matrix;
            DrawModel(models[MODEL_QUEEN], position, 0.015f, WHITE);
            break;
        default:
            break;
    }
}

void game_board_models_load(game_state_t* game_state){
    models[MODEL_BISHOP] = LoadModel("assets/bishop.obj");
    models[MODEL_BISHOP].materials[0].shader = game_state->shader;

    models[MODEL_ROOK] = LoadModel("assets/rook.obj");
    models[MODEL_ROOK].materials[0].shader = game_state->shader;

    models[MODEL_PAWN] = LoadModel("assets/pawn.obj");
    models[MODEL_PAWN].materials[0].shader = game_state->shader;

    models[MODEL_KING] = LoadModel("assets/king.obj");
    models[MODEL_KING].materials[0].shader = game_state->shader;

    models[MODEL_QUEEN] = LoadModel("assets/queen.obj");
    models[MODEL_QUEEN].materials[0].shader = game_state->shader;

    models[MODEL_KNIGHT] = LoadModel("assets/knight.obj");
    models[MODEL_KNIGHT].materials[0].shader = game_state->shader;
}

void game_board_draw(game_state_t *game_state)
{
    float offsetX = 0.0f;
    float offsetZ = 0.0f;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Vector3 pos = (Vector3){i + offsetX, -0.5f, j + offsetZ};
            game_board_pieces_draw(game_state->board[j][i], pos);
            DrawCube(pos, 1.0f, 0.2f, 1.0f, ((i + j) % 2) ? DARKGRAY : WHITE);
        }
    }
}

#endif