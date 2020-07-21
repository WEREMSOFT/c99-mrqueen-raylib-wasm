#include <time.h>
#include "bb.h"
#include "uci.h"
#include "util.h"
#include <unistd.h>

int main(int argc, char **argv) {
    bb_init();
    prng_seed(time(NULL));
    uci_main();

    sleep(5);

    return 0;
}
