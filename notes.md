#include <stdio.h>
#include "console_output.h"


void uci_main() {
    board_reset(&board);

    char commands[COMMAND_COUNT][1000] = { "uci", "ucinewgame", "isready", "position startpos moves d2d4", "go wtime 300000 btime", "quit"};

    for(int i = 0; i < COMMAND_COUNT; i++){
        parse_line(commands[i]);
    }

    // while (parse_line());
}