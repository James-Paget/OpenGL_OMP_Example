#include "entity.h"
#include "GLFW/glfw3.h"

#ifndef MANAGER_H
#define MANAGER_H

void manage_simulation_main(GLFWwindow *window, const unsigned int screen_width, const unsigned int screen_height);
void populate_entities(Entity *entities, const unsigned int entity_number, float *spawn_dimemsion);
float rand_float(int seed_offset);

#endif
