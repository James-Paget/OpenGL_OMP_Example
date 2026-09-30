#include "entity.h"
#include "creature.h"
#include "resource.h"
#include "GLFW/glfw3.h"

#ifndef MANAGER_H
#define MANAGER_H

void manage_simulation_main(GLFWwindow *core_window, GLFWwindow *hud_window, const unsigned int screen_width, const unsigned int screen_height);

void render_core(GLFWwindow *core_window, unsigned int shaderProgram, unsigned int VAO_entity, Creature *creatures, const unsigned int creature_number, Resource *resources, const unsigned int resource_number, const unsigned int screen_width, const unsigned int screen_height, float *prior_t, float *delta_t);
void render_hud(GLFWwindow *hud_window, unsigned int shaderProgram, unsigned int VAO_entity, Creature *creatures, const unsigned int creature_number, const unsigned int screen_width, const unsigned int screen_height);

void populate_creatures(Creature *creatures, const unsigned int creature_number, float *spawn_dimemsion);
void populate_resources(Resource *resources, const unsigned int resource_number, float *spawn_dimemsions);
float rand_float(int seed_offset);

#endif
