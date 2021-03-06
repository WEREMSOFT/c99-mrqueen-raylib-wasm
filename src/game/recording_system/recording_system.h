#ifndef __RECORDING_SYSTEM_H__
#define __RECORDING_SYSTEM_H__

#include <msf_gif.h>
#include <time.h>
#include <raylib.h>
#include <stdbool.h>


enum recording_system_states_e {
    RECORDING_SYSTEM_STATE_READY,
    RECORDING_SYSTEM_STATE_RECORDING
};

typedef struct recording_system_t {
    unsigned int state;
    int centiseconds_per_frame;
    int bit_depth;
    clock_t base_time;
    Image img;
    MsfGifState gif_state;
} recording_system_t;

void recording_system_add_frame(void);
void recording_system_init(void);
void recording_system_stop_recording(void);
void recording_system_start_recording(void);
void recording_system_update(void);
void recording_system_fini(void);
bool recording_system_is_ready(void);
unsigned int recording_system_state(void);
#endif