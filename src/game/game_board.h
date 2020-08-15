#ifndef __GAME_BOARD_H__
#define __GAME_BOARD_H__

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

#endif