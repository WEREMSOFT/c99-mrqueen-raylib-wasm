#ifndef __GAME_BOARD_H__
#define __GAME_BOARD_H__
#include <stdbool.h>
#include <raygui.h>
#include "game.h"

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

char board_coord_x[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
char board_coord_y[8] = {'8', '7', '6', '5', '4', '3', '2', '1'};

const unsigned int base_board[8][8] = {{TWR_B, KGT_B, BSP_B, QEN_B, KNG_B, BSP_B, KGT_B, TWR_B},
                            {PWN_B, PWN_B, PWN_B, PWN_B, PWN_B, PWN_B, PWN_B, PWN_B},
                            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                            {PWN_W, PWN_W, PWN_W, PWN_W, PWN_W, PWN_W, PWN_W, PWN_W},
                            {TWR_W, KGT_W, BSP_W, QEN_W, KNG_W, BSP_W, KGT_W, TWR_W},
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

Model models[MODEL_COUNT] = {0};

typedef struct game_board_matrix_coordinates_t {
    unsigned int source_x;
    unsigned int source_y;
    unsigned int target_x;
    unsigned int target_y;

} game_board_matrix_coordinates_t;

void game_board_reset(unsigned int board[8][8]){
    memcpy(board, base_board, sizeof(board) * 64);
}

void game_board_calculate_attacked_positions(unsigned int board[8][8], unsigned int attack_board[8][8]){
    memset(attack_board, 0x00, sizeof(attack_board) * 64);
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            switch (board[j][i])
            {
                case PWN_B:
                    if(j+1 < 8 && i+1 < 8){
                        attack_board[j+1][i+1] = true;
                    }
                    if(j+1 < 8 && i-1 >= 0){
                        attack_board[j+1][i-1] = true;
                    }
                    break;
                case KGT_B:
                    if(j+2 < 8 && i+1 < 8){
                        attack_board[j+2][i+1] = true;
                    }
                    if(j+2 < 8 && i-1 >= 0){
                        attack_board[j+2][i-1] = true;
                    }
                    if(j+1 < 8 && i-2 >= 0){
                        attack_board[j+1][i-2] = true;
                    }
                    if(j+1 < 8 && i+2 < 8){
                        attack_board[j+1][i+2] = true;
                    }

                    if(j-2 >= 0 && i-1 >= 0){
                        attack_board[j-2][i-1] = true;
                    }
                    if(j-2 >= 0 && i+1 < 8){
                        attack_board[j-2][i+1] = true;
                    }
                    if(j-1 >= 0 && i+2 < 8){
                        attack_board[j-1][i+2] = true;
                    }
                    if(j-1 >= 0 && i-2 >= 0){
                        attack_board[j-1][i-2] = true;
                    }
                    break;
                case BSP_B:
                    {
                        int pos_j = j;
                        int pos_i = i;
                        while(pos_j+1 < 8 && pos_i+1 < 8){
                            attack_board[++pos_j][++pos_i] = true;
                            if(board[pos_j][pos_i] != EMPTY) break;
                        }
                        pos_j = j;
                        pos_i = i;
                        while(pos_j+1 < 8 && pos_i-1 >= 0){
                            attack_board[++pos_j][--pos_i] = true;
                            if(board[pos_j][pos_i] != EMPTY) break;
                        }
                    }
                    break;
                case TWR_B:
                    {
                        int pos_j = j;
                        int pos_i = i;
                        while(pos_j+1 < 8){
                            attack_board[++pos_j][pos_i] = true;
                            if(board[pos_j][pos_i] != EMPTY) break;
                        }
                        pos_j = j;
                        pos_i = i;
                        while(pos_j-1 >= 0){
                            attack_board[--pos_j][pos_i] = true;
                            if(board[pos_j][pos_i] != EMPTY) break;
                        }
                        pos_j = j;
                        pos_i = i;
                        while(pos_i+1 < 8){
                            attack_board[pos_j][++pos_i] = true;
                            if(board[pos_j][pos_i] != EMPTY) break;
                        }
                        pos_j = j;
                        pos_i = i;
                        while(pos_i-1 >= 0){
                            attack_board[pos_j][--pos_i] = true;
                            if(board[pos_j][pos_i] != EMPTY) break;
                        }
                    }
                    break;
            }
        }
    }
}

game_board_matrix_coordinates_t game_board_get_coordinates_in_matrix(char *coords){
    game_board_matrix_coordinates_t return_value = {0};

    return_value.source_x = (unsigned int)(strchr(board_coord_x, coords[0]) - board_coord_x);
    return_value.source_y = (unsigned int)(strchr(board_coord_y, coords[1]) - board_coord_y);
    return_value.target_x = (unsigned int)(strchr(board_coord_x, coords[2]) - board_coord_x);
    return_value.target_y = (unsigned int)(strchr(board_coord_y, coords[3]) - board_coord_y);

    return return_value;
}

Vector3 game_board_get_target_coordinates_in_world(char *coords){
    game_board_matrix_coordinates_t coords_m = {0};

    coords_m = game_board_get_coordinates_in_matrix(coords);

    Vector3 return_value = (Vector3){coords_m.target_x , -0.5f, coords_m.target_y };

    return return_value;
}

Vector3 game_board_get_source_coordinates_in_world(char *coords){
    game_board_matrix_coordinates_t coords_m = {0};

    coords_m = game_board_get_coordinates_in_matrix(coords);

    Vector3 return_value = (Vector3){coords_m.source_x , -0.5f, coords_m.source_y };

    return return_value;
}

unsigned int game_board_get_piece_at_target(unsigned int board[8][8], char *coords){
    game_board_matrix_coordinates_t coords_m = game_board_get_coordinates_in_matrix(coords);
    
    return board[coords_m.target_y][coords_m.target_x];
}

unsigned int game_board_get_piece_at_source(unsigned int board[8][8], char *coords){
    game_board_matrix_coordinates_t coords_m = game_board_get_coordinates_in_matrix(coords);
    
    return board[coords_m.source_y][coords_m.source_x];
}

void game_board_set_piece_at_source(unsigned int board[8][8], char *coords, unsigned int piece){
    game_board_matrix_coordinates_t coords_m = game_board_get_coordinates_in_matrix(coords);

    board[coords_m.source_y][coords_m.source_x] = piece;
}

void game_board_set_piece_at_target(unsigned int board[8][8], char *coords, unsigned int piece){
    game_board_matrix_coordinates_t coords_m = game_board_get_coordinates_in_matrix(coords);

    board[coords_m.target_y][coords_m.target_x] = piece;
}

void game_board_move_piece(unsigned int board[8][8], char *coords)
{
    game_board_matrix_coordinates_t coords_m = game_board_get_coordinates_in_matrix(coords);
    
    board[coords_m.target_y][coords_m.target_x] = board[coords_m.source_y][coords_m.source_x];
    board[coords_m.source_y][coords_m.source_x] = EMPTY;
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
        case PIECE_IN_MOTION:
            // we never draw this.
            break;
        default:
            break;
    }
}

void game_board_models_load(Shader shader){
    models[MODEL_BISHOP] = LoadModel("assets/bishop.obj");
    models[MODEL_BISHOP].materials[0].shader = shader;

    models[MODEL_ROOK] = LoadModel("assets/rook.obj");
    models[MODEL_ROOK].materials[0].shader = shader;

    models[MODEL_PAWN] = LoadModel("assets/pawn.obj");
    models[MODEL_PAWN].materials[0].shader = shader;

    models[MODEL_KING] = LoadModel("assets/king.obj");
    models[MODEL_KING].materials[0].shader = shader;

    models[MODEL_QUEEN] = LoadModel("assets/queen.obj");
    models[MODEL_QUEEN].materials[0].shader = shader;

    models[MODEL_KNIGHT] = LoadModel("assets/knight.obj");
    models[MODEL_KNIGHT].materials[0].shader = shader;
}

void game_board_models_unload(){
    for(int i = 0; i < MODEL_COUNT; i++){
        UnloadModel(models[i]);
    }
}

void game_board_draw(unsigned int board[8][8])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Vector3 pos = (Vector3){i , -0.5f, j };
            game_board_pieces_draw(board[j][i], pos);
            DrawCube(pos, 1.0f, 0.2f, 1.0f, ((i + j) % 2) ? DARKGRAY : LIGHTGRAY);
        }
    }
}

void game_board_attaked_positions_draw(unsigned int board[8][8])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Vector3 pos = (Vector3){i , -0.5f, j };
            if(board[j][i])
                DrawCube(pos, 1.0f, 0.3f, 1.0f, BROWN);
        }
    }
}

#endif