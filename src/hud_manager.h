#ifndef HUD_MANAGER_H
#define HUD_MANAGER_H

#include "GLFW/glfw3.h"
#include "vector"

#include "Graphical_Elements/ge_bar.h"
#include "Graphical_Elements/ge_linegraph.h"

class HUD_Manager {
    /*
    . Stores data about the HUD window, such as variables needed for visualisation or calculation values specific to this window alone
    . Controls the initial setup & the per-frame rendering of the window
    . All relevent shaders, buffers, etc are stored indepently in each window manager class (Core, HUD, etc)
    */
    public:
        const float refresh_rate = 0.1f;     // Time between HUD refreshes
        float prior_refresh_time = -refresh_rate;   // What time (glfwGetTime()) was the HUD previously updated on
        Graphical_Element *graphical_elements[3];

        HUD_Manager();
        ~HUD_Manager();
        void setup();
        void render(GLFWwindow *window);
    private:
        //pass
};

#endif
