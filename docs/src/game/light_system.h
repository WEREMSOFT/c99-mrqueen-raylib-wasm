#ifndef __LIGHT_SYSTEM_H__
#define __LIGHT_SYSTEM_H__
#include "game_context.h"

#if defined(OS_WEB)
#define GLSL_VERSION            100
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            330
#endif

void lights_init(game_context_t* game_state){
    game_state->shader = LoadShader(FormatText("./assets/shaders/glsl%i/base_lighting.vs", GLSL_VERSION),
                                FormatText("./assets/shaders/glsl%i/lighting.fs", GLSL_VERSION));

    game_state->shader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(game_state->shader, "matModel");
    game_state->shader.locs[LOC_VECTOR_VIEW] = GetShaderLocation(game_state->shader, "viewPos");

    int ambientLoc = GetShaderLocation(game_state->shader, "ambient");
    SetShaderValue(game_state->shader, ambientLoc, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, UNIFORM_VEC4);

    game_state->light1 = CreateLight(LIGHT_POINT, (Vector3){ 6.7f, 4, 6.4f }, Vector3Zero(), GRAY, game_state->shader);
    game_state->light2 = CreateLight(LIGHT_POINT, (Vector3){ .2f, 4, 5.1f }, Vector3Zero(), GRAY, game_state->shader);
    game_state->light3 = CreateLight(LIGHT_POINT, (Vector3){ 4.0f, 4, 11.0f }, Vector3Zero(), GRAY, game_state->shader);
    UpdateLightValues(game_state->shader, game_state->light1);
    UpdateLightValues(game_state->shader, game_state->light2);
    UpdateLightValues(game_state->shader, game_state->light3);
}


#endif