#include <stdio.h>
#include <string.h>
#include "unity/unity.h"
// This trick allows us to test our main program file.
// We undefine the main function so it will not collide with the test main.
#define main _main
#include "../src/game/event.h"
#include "../src/game/event.c"
#undef main

void setUp(void)
{
    // printf("This is run before EACH TEST\n");
}

void tearDown(void)
{
    // printf("This is run after EACH TEST\n");
}

void test_queue(void)
{
    printf("should dequeue in order\n");
    event_t evt = {0};
    evt.type = EVENT_COMMAND;
    strcpy(evt.data, "this is a test");
    event_queue(evt);

    strcat(evt.data, ", 2nd test");
    event_queue(evt);

    strcat(evt.data, ", 3rd test");
    event_queue(evt);

    event_t out_event = {0};
    out_event = event_dequeue();
    TEST_ASSERT_EQUAL_STRING("this is a test", out_event.data);
    out_event = event_dequeue();
    TEST_ASSERT_EQUAL_STRING("this is a test, 2nd test", out_event.data);
    out_event = event_dequeue();
    TEST_ASSERT_EQUAL_STRING("this is a test, 2nd test, 3rd test", out_event.data);

    out_event = event_dequeue();
    TEST_ASSERT_EQUAL_INT32(EVENT_EMPTY, out_event.type);

    strcpy(evt.data, "this is a test2");
    event_queue(evt);

    strcat(evt.data, ", 2nd test");
    event_queue(evt);

    strcat(evt.data, ", 3rd test");
    event_queue(evt);

    out_event = event_dequeue();
    TEST_ASSERT_EQUAL_STRING("this is a test2", out_event.data);
    out_event = event_dequeue();
    TEST_ASSERT_EQUAL_STRING("this is a test2, 2nd test", out_event.data);
    out_event = event_dequeue();
    TEST_ASSERT_EQUAL_STRING("this is a test2, 2nd test, 3rd test", out_event.data);
    
    out_event = event_dequeue();
    TEST_ASSERT_EQUAL_INT32(EVENT_EMPTY, out_event.type);
}

void test_queue_full(void)
{
    printf("should queue until is full\n");
    event_t evt = {0};
    evt.type = EVENT_COMMAND;
    
    int i = 0;

    while(event_queue(evt) != -1)
    {
        sprintf(evt.data, "Data is: %d", i);
        i++;
    }

    TEST_ASSERT_EQUAL_INT32(100, i);

    while((evt = event_dequeue()).type != EVENT_EMPTY)
        i--;

    TEST_ASSERT_EQUAL_INT32(0, i);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_queue);
    RUN_TEST(test_queue_full);
    return UNITY_END();
}