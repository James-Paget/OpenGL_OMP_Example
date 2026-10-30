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

    // DoneShortList = Clean-up classes /w core_manager, setting up assimp, setting up models pipeline using guide, 
    //                 Adding basic outpost /w textures, fixed random, generate cleanup for headers, moved old example_multi 
    //                 files which were used initialy for helping me remember function names in GLFW

// ### (1) Ballistic throwing setup / talking to eachother
// ### (2) Increase resolution of the ping sphere
// ### (3) More controlled pings for testing
// ###  --> Fix issue with ping triggering resources far away

// ### (4) textures + lighting for OpenGL
// ### (5) OMP for loading normals for lighting (even-odd lighting pairing issue investigate)

// ### (6) Write a file read-write system for reading shader files -> cleaner, keep separate
