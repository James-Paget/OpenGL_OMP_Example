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

        Graphical_Element(glm::vec3 origin, glm::vec3 dimensions);
        virtual void setup_full();
        virtual void assign_shader();
        virtual void bind_VAO();
        virtual void render();
    private:
        //pass
};

#endif
