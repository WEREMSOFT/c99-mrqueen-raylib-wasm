#ifndef __GAME_OPTIONS_H__
#define __GAME_OPTIONS_H__

typedef struct game_options_t {
    bool draw_attacked_positions;
    bool allow_move_if_king_is_pinned;
} game_options_t;

#endif