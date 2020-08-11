#include "event.h"
#include <stdio.h>
#include <stdbool.h>

#define EVENT_QUEUE_SIZE 100

void queue_init() {
    events.back = events.front = -1;
}

bool queue_is_empty() {
    return events.front == -1;
}

bool queue_is_full() {
    return (events.front == events.back + 1) || (events.front == 0 && events.back == EVENT_QUEUE_SIZE -1);
}

int event_queue(event_t event) {
    if(queue_is_full()) return -1;
    
    if(events.front == -1) events.front = 0;

    events.back = (events.back + 1) % EVENT_QUEUE_SIZE;

    events.events[events.back] = event;

    return 0;
}

event_t event_dequeue() {
    event_t return_value = {0};
    if(queue_is_empty()) return return_value;
    
    return_value = events.events[events.front];

    if(events.front == events.back)
        events.back = events.front = -1;
    else 
        events.front = (events.front + 1) % EVENT_QUEUE_SIZE;

    return return_value;
}

#undef EVENT_QUEUE_SIZE
