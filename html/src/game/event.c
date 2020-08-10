#include "event.h"
#include <stdio.h>
#include <stdbool.h>

#define EVENT_QUEUE_SIZE 100

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
    if(events.size == 0) events.front = events.back = 0;

    return return_value;
}

#undef EVENT_QUEUE_SIZE
