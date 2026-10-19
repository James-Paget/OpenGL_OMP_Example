#include "GLFW/glfw3.h"
#include "vector"

#include "Graphical_Elements/ge_bar.h"

#ifndef HUD_MANAGER_H
#define HUD_MANAGER_H

class HUD_Manager {
    /*
    . Stores data about the HUD window, such as variables needed for visualisation or calculation values specific to this window alone
    . Controls the initial setup & the per-frame rendering of the window
    . All relevent shaders, buffers, etc are stored indepently in each window manager class (Core, HUD, etc)
    */
    public:
        // std::vector<Graphical_Element>
        // ###
        // ### Store pointers to the class
        // ### When populating this in a function DYNAMICALL ALLOCATE the memory by using 'new' --> Makes space for it rather than leaving it on the stack
        // ###
        // ###      BUT this means the dynamically allocated memory has to be deleted manually after use
        // ###          E.g. 'obj_ptr delete' will remove the data at the pointer (e.g. if you remove one of these pointers from the list it will need to delete what it left or you will have memory leaks)
        // ###      -> YOU SHOULD PROBABLY HAVE A DESTRUCTOR CLASS FUNC TOO
        // ###
        GE_Bar *element_ge_bar = new GE_Bar( glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.5f, 0.0f) );

        HUD_Manager();
        void setup();
        void render(GLFWwindow *window);
    private:
        // ### MOVE THESE INTO THE GRAPHICAL ELEMENTS / PARSE INTO GRAPHICAL ELEMENTS [IF] VALID FORMAT
        //pass
};

#endif

// ###
// ### Make a separate 'GraphicalElement' class
// ###      Sub-classes for bar graphs, line graphs, stats, etc
// ###          -> Use common variables for origin, dimensions --> REST CAN BE HYPER-SPECIFIC TO THAT FIGURE
