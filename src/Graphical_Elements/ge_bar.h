#include "graphical_element.h"

#ifndef GE_BAR_H
#define GE_BAR_H

class GE_Bar : public Graphical_Element {
    public:
        //pass

        GE_Bar(glm::vec3 origin, glm::vec3 dimensions);
        void setup();
        void assign_shader();
        void bind_VAO();
        void render();
    private:
        const char *vertex_shader_GE_BAR_H = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = vec4(aPos, 1.0);\n"
            "}\0";

        const char *fragment_shader_GE_BAR_H = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "void main()\n"
            "{\n"
            "   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
            "}\0";

        unsigned int shaderProgram = 0;         // Initialised to 0 to check are being corretcly assigned -> Was getting seg.faults possibly due to this
        unsigned int vertexShaderID = 0;    // ### THESE GET DELETED ANYWAY SO CAN PROB JUST BE LOCAL TO setup() ###
        unsigned int fragmentShaderID = 0;  // ###
        unsigned int VBO = 0;
        unsigned int VAO = 0;
        unsigned int EBO = 0;

        float vertex_data[12] = {
            -0.25f,  0.25f, 0.0f,
             0.25f,  0.25f, 0.0f,
            -0.25f, -0.25f, 0.0f,
             0.25f, -0.25f, 0.0f
        };
        // ###
        // ### TRY USE DIMENSIONS HERE -> WILL THEY INITIALISE AT THE SAME TIME AND WORK??
        // ###
        // ###  ALSO INTELLI-SENSE SEEMS TO BE BREAKING HERE -> TRY FIX
        // ###
        // ######
        // ### CHANGE TO DYNAMIC DRAW IN SETUP AFTER THE STATIC TEST
        // ######
        int vertex_indices[6] = {
            0, 1, 2,
            2, 3, 4
        };
};

#endif
