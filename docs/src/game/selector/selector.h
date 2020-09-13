#ifndef __SELECTOR_H__
#define __SELECTOR_H__

#include <raylib.h>
#include <stdbool.h>

enum enum_selector_state {
    SELECTOR_STATE_DISABLED,
    SELECTOR_STATE_READY,
    SELECTOR_STATE_AWAITING_TARGET,
    SELECTOR_STATE_ILLEGAL,
};

typedef struct selector_t {
    Vector3 position;
    Vector3 position_start;
    unsigned int state;
    unsigned int origin_piece;
    Vector2 white_king_position;
    unsigned int board_attacked_positions[8][8];
    Color color;
    bool visible;
} selector_t;

void selector_process_keys(selector_t *selector);
void selector_pass_to_state_illegal(selector_t* selector);
void selector_send_move_to_engine(const selector_t selector);
void selector_pass_to_state_ready(selector_t* selector);
void selector_process_state_illegal(selector_t* selector);
void selector_draw(selector_t selector);
bool selector_is_origin_position_legal(selector_t selector, unsigned int board[8][8]);
bool selector_is_target_position_legal(selector_t selector, unsigned int board[8][8]);
void selector_process_state_awaiting_target(selector_t* selector, unsigned int board[8][8]);
void selector_process_state_ready(selector_t* selector, unsigned int board[8][8]);
void selector_update(selector_t* selector, unsigned int board[8][8], unsigned int board_attacked_positions[8][8]);
#endif