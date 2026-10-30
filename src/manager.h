#ifndef MANAGER_H
#define MANAGER_H

#include "entity.h"
#include "creature.h"
#include "resource.h"
#include "GLFW/glfw3.h"

#include "core_manager.h"
#include "hud_manager.h"

void manage_simulation_main(GLFWwindow *core_window, GLFWwindow *hud_window, const unsigned int screen_width, const unsigned int screen_height);
void refresh_hud(HUD_Manager *_hud_manager, Core_Manager *_core_manager);

#endif
