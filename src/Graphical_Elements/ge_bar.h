#include "graphical_element.h"

#ifndef GE_BAR_H
#define GE_BAR_H

class GE_Bar : public Graphical_Element {
    public:
        // Origin at centre of bar
        // Dimensions = Full width/height
        static const uint VALUE_MAXIMUM = 10;   // Maximum of 10 different values per bar figure
        float bar_values[VALUE_MAXIMUM];        // The different values making up the bar's total
        int number_of_values = 0;               // How many values are currently being tracked

        GE_Bar(glm::vec3 origin, glm::vec3 dimensions);
        ~GE_Bar();
        void setup_full() override;
        void setup_shader();
        void setup_refresh();
        void generate_vertex_info();
        void assign_shader() override;
        void bind_VAO() override;
        void render() override;

        void refresh_data(float *input_bar_values, int length);

        void print_vertex_info();
    private:
        const char *vertex_shader_GE_BAR_H = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = vec4(aPos, 1.0);\n"
            "}\0";

        const char *fragment_shader_GE_BAR_H = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "uniform vec3 sectorColour;\n"
            "void main()\n"
            "{\n"
            "   FragColor = vec4(sectorColour, 1.0f);\n"
            "}\0";

        unsigned int shaderProgram = 0;         // Initialised to 0 to check are being corretcly assigned -> Was getting seg.faults due to this + window context
        unsigned int VBO = 0;
        unsigned int VAO = 0;
        unsigned int EBO = 0;

        float vertex_data[6*(VALUE_MAXIMUM+1)];
        int vertex_indices[6*VALUE_MAXIMUM];
};

#endif
