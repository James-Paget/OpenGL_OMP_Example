#include<iostream>
#include "glad.h"
#include<GLFW/glfw3.h>
#include <mpi.h>

#include "simple_triangle_example.h"
#include "custom_multi_example.h"

int main(int argc, char *argv[]) {
    /*
    ADDITIONAL NOTES**
        . Must add new src files to CMakeLists to compile together correctly
        . GLFW for window creation, GLAD for OpenGL rendering extras it appears
            +Include GLAD before GLFW to prevent errors
    */
    GLFWwindow* window;

    if( !glfwInit() ) {     // Initialise GLFW, else leave with error code
        std::cout << "GLFW failed to load" << std::endl;
        return -1;
    }

    const unsigned int screen_width = 800;
    const unsigned int screen_height = 600;
    window = glfwCreateWindow(screen_width, screen_height, "WINDOW_NAME", NULL, NULL);
    glfwMakeContextCurrent(window);

    // **NOTE; Must be done AFTER GLFW initialised (hence must terminate too if there is an error)
    if( !gladLoadGLLoader((GLADloadproc) glfwGetProcAddress) ) {        // Loading GLAD info, else leave with error code
        std::cout << "GLAD failed to load" << std::endl;                // Must also be run AFTER making the window current, otherwise will NOT load correctly
        glfwTerminate();
        return -1;
    }

    // Testing MPI
    /*
    . Use "mpirun -np 2 ./EXE_FILE" if you want to utilise MPI
    */
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("Hello, World.  I am %d of %d\n", rank, size);
    MPI_Finalize();
    // Testing MPI

    // --- RUN PROGRAMS WITHIN HERE ---
    // perform_simple_triangle_example(window);
    // perform_simple_triangle_example_ex1(window);
    // perform_simple_triangle_example_ex2(window);
    perform_custom_multi_example_A(window, screen_width, screen_height);
    // --- RUN PROGRAMS WITHIN HERE ---

    glfwTerminate();    // Stop GLFW before program end - clean-up
    return 0;
}



// ###
// ### (1) Render shapes + lighting
// ### (2) MOI add details 
// ### (3) MT Comp. in Cpp 
// ###