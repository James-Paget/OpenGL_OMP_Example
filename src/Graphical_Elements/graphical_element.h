#include "GLFW/glfw3.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

#ifndef GRAPHICAL_ELEMENT_H
#define GRAPHICAL_ELEMENT_H

class Graphical_Element {
    public:
        glm::vec3 origin;       // Location that the new graphical element will sit at (depends on element type; usually a top-left corner OR central point)
        glm::vec3 dimensions;   // Width/Height/Depth of graphical element (varies depending on element type)

        // ###
        // ### NO SIZE GIVEN YET -> IS AN ISSUE FOR SETTING CORRECT MEMORY SIZE FOR THIS
        // ###      -> MUST NEED A VALUE STRAIGHT AWAY NO??
        // ###          -> HENCE SHOULD GIVE IN THE SUB-CLASS
        // ###
        // float vertex_data[];
        // int vertex_indices[];

        Graphical_Element(glm::vec3 origin, glm::vec3 dimensions);
        void render();
    private:
        //pass
};

#endif
