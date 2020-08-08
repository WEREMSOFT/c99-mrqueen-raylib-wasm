#ifndef __EVENT_H__
#define __EVENT_H__

#include <stdio.h>
#include <stdbool.h>

#define EVENT_QUEUE_SIZE 100

enum event_type_e {
    EVENT_EMPTY,
    EVENT_COMMAND,
    EVENT_RESPONSE,
    EVENT_COUNT
};

typedef struct event_t {
    unsigned int type;
    char data[100];
} event_t;


typedef struct event_queue_t {
    int front, back, size;
    event_t events[EVENT_QUEUE_SIZE];
} event_queue_t;

static event_queue_t events = {0};

bool queue_is_empty() {
    return (events.size == 0);
}

bool queue_is_full() {
    return events.size == EVENT_QUEUE_SIZE;
}

int event_queue(event_t event) {
    if(queue_is_full()) return -1;
    
    if(!queue_is_empty())
        events.back = (events.back+1) % EVENT_QUEUE_SIZE;
    
    events.events[events.back] = event;
    
    events.size++;
    return 0;
}

event_t event_dequeue() {
    event_t return_value = {0};
    if(queue_is_empty()) return return_value;
    
    return_value = events.events[events.front];
    events.front = (events.front + 1) % EVENT_QUEUE_SIZE;
    events.size--;

    return return_value;
}

#undef EVENT_QUEUE_SIZE;

#endif