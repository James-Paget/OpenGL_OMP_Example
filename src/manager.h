#include "entity.h"
#include "GLFW/glfw3.h"

#ifndef MANAGER_H
#define MANAGER_H

void manage_simulation_main(GLFWwindow *core_window, GLFWwindow *hud_window, const unsigned int screen_width, const unsigned int screen_height);

void render_core(GLFWwindow *core_window, unsigned int shaderProgram, unsigned int VAO_entity, Entity *entities, const unsigned int entity_number, const unsigned int screen_width, const unsigned int screen_height, float *prior_t, float *delta_t);
void render_hud(GLFWwindow *hud_window, unsigned int shaderProgram, unsigned int VAO_entity, const unsigned int screen_width, const unsigned int screen_height);

void populate_entities(Entity *entities, const unsigned int entity_number, float *spawn_dimemsion);
float rand_float(int seed_offset);

#endif
