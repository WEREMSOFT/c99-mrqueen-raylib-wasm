#include "event.h"
#include <stdio.h>
#include <stdbool.h>

#define EVENT_QUEUE_SIZE 100
static event_queue_t events = {0};

void event_queue_init() {
    events.back = events.front = -1;
}

bool event_queue_is_empty() {
    return events.front == -1;
}

bool event_queue_is_full() {
    return (events.front == events.back + 1) || (events.front == 0 && events.back == EVENT_QUEUE_SIZE -1);
}

int event_queue_enqueue(event_t event) {
    if(event_queue_is_full()) return -1;
    
    if(events.front == -1) events.front = 0;

    events.back = (events.back + 1) % EVENT_QUEUE_SIZE;

    events.events[events.back] = event;

    return 0;
}

event_t event_queue_dequeue() {
    event_t return_value = {0};
    if(event_queue_is_empty()) return return_value;
    
    return_value = events.events[events.front];

    if(events.front == events.back)
        events.back = events.front = -1;
    else 
        events.front = (events.front + 1) % EVENT_QUEUE_SIZE;

    return return_value;
}

#undef EVENT_QUEUE_SIZE
