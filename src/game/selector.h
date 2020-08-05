#include <raylib.h>

enum enum_selector_state {
    SELECTOR_STATE_DISABLED,
    SELECTOR_STATE_READY,
    SELECTOR_STATE_AWAITING_TARGET,
};

typedef struct selector_t {
    Vector3 position;
    Vector3 position_start;
    unsigned int state;
} selector_t;

void selector_process_state_ready(selector_t* selector){
    if (IsKeyPressed(KEY_LEFT))
    {
        selector->position.x -= 1.0f;
    }

    if (IsKeyPressed(KEY_RIGHT))
    {
        selector->position.x += 1.0f;
    }

    if (IsKeyPressed(KEY_UP))
    {
        selector->position.z -= 1.0f;
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        selector->position.z += 1.0f;
    }

    if(IsKeyPressed(KEY_SPACE))
    {
        selector->state = SELECTOR_STATE_AWAITING_TARGET;
        selector->position_start = selector->position;
    }

    DrawCubeWires(selector->position, 1, 1, 1, RED);
}

void selector_process_state_awaiting_target(selector_t* selector)
{
    if (IsKeyPressed(KEY_LEFT))
    {
        selector->position.x -= 1.0f;
    }

    if (IsKeyPressed(KEY_RIGHT))
    {
        selector->position.x += 1.0f;
    }

    if (IsKeyPressed(KEY_UP))
    {
        selector->position.z -= 1.0f;
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        selector->position.z += 1.0f;
    }
    DrawCubeWires(selector->position, 1, 1, 1, RED);
    DrawCubeWires(selector->position_start, 1, 1, 1, PURPLE);
}

void selector_draw(selector_t* selector)
{
    switch(selector->state){
        case SELECTOR_STATE_READY:
            selector_process_state_ready(selector);
            break;
        case SELECTOR_STATE_AWAITING_TARGET:
            selector_process_state_awaiting_target(selector);
            break;
    }
}