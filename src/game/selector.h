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

void selector_draw(selector_t* selector)
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
}