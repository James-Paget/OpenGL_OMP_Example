#include<iostream>
#include "glad.h"
#include<GLFW/glfw3.h>
#include <mpi.h>

#include "manager.h"

const unsigned int screen_width = 800;
const unsigned int screen_height = 600;

int main(int argc, char *argv[]) {
    GLFWwindow* core_window;
    GLFWwindow* hud_window;

    if( !glfwInit() ) {     // Initialise GLFW, else leave with error code
        std::cout << "GLFW failed to load" << std::endl;
        return -1;
    }

    core_window = glfwCreateWindow(screen_width, screen_height, "CORE", NULL, NULL);
    hud_window = glfwCreateWindow(screen_width, screen_height>>1, "HUD", NULL, NULL);
    glfwMakeContextCurrent(core_window);

    // **NOTE; Must be done AFTER GLFW initialised (hence must terminate too if there is an error)
    if( !gladLoadGLLoader((GLADloadproc) glfwGetProcAddress) ) {        // Loading GLAD info, else leave with error code
        std::cout << "GLAD failed to load" << std::endl;                // Must also be run AFTER making the window current, otherwise will NOT load correctly
        glfwTerminate();
        return -1;
    }

    manage_simulation_main(core_window, hud_window, screen_width, screen_height);

    glfwTerminate();    // Stop GLFW before program end - clean-up
    return 0;
}


// ### (1) Models + textures + lighting for OpenGL
// ### (2) OMP for loading normals for lighting (even-odd lighting pairing issue investigate)

// ### (3) Ballistic throwing setup / talking to eachother
// ### (4) Increase resolution of the ping sphere

// ### (5) More controlled pings for testing
// ###  --> Fix issue with ping triggering resources far away

// ### (6) Clean program structure up in general -> removal experimental testing and messy layout + group blocks that are now working correctly
// ### (7) Write a file read-write system for reading shader files -> cleaner, keep separate
// ### (8) Ballistic throwing to move found resources / harvest them
