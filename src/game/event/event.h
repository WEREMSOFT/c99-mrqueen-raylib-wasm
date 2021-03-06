#ifndef __EVENT_H__
#define __EVENT_H__

#include <stdio.h>
#include <stdbool.h>

#define EVENT_QUEUE_SIZE 100

enum event_type_e {
    EVENT_EMPTY,
    EVENT_COMMAND,
    EVENT_RESPONSE,
    EVENT_UI_NEW_GAME,
    EVENT_LOG,
    EVENT_COUNT
};

typedef struct event_t {
    unsigned int type;
    char data[200];
} event_t;


typedef struct event_queue_t {
    int front, back, size;
    event_t events[EVENT_QUEUE_SIZE];
} event_queue_t;

void event_queue_init(void);
bool event_queue_is_empty(void);
bool event_queue_is_full(void);
int event_queue_enqueue(event_t event);
event_t event_queue_dequeue(void);

#undef EVENT_QUEUE_SIZE

#endif