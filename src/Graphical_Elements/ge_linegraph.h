#include <vector>
#include "graphical_element.h"

#ifndef GE_LINEGRAPH_H
#define GE_LINEGRAPH_H

class GE_Linegraph : public Graphical_Element {
    public:
        // ###
        // ### EXTEND THIS TO DO N LINES PER FIGURE
        // ###
        // Origin bottom-left corner / intersection point for figure
        // Dimensions width/height of graph (scaleless)
        static const int DATA_LIMIT_MAX = 50;  // Max number of data points that can be displayed in the figure at once

        float x_scale_start = 0.0f;     //       | (4)
        float x_scale_end   = 0.0f;     //       |
        float y_scale_start = 0.0f;     //       |
        float y_scale_end   = 0.0f;     // (1,3)-|------- (2)

        std::vector<float> data_x;  // Combined gives (x,y) data points
        std::vector<float> data_y;  //

        GE_Linegraph(glm::vec3 origin, glm::vec3 dimensions, float x_scale_start, float x_scale_end, float y_scale_start, float y_scale_end);
        ~GE_Linegraph();

        void setup_full() override;
        void setup_shader();
        void setup_refresh();
        void generate_vertex_info();
        void assign_shader() override;
        void bind_VAO() override;
        void render() override;

        void refresh_data(float *input_data_x, float *input_data_y, int length);
    private:
        const char *vertex_shader_GE_LINEGRAPH_H = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = vec4(aPos, 1.0);\n"
            "}\0";

        const char *fragment_shader_GE_LINEGRAPH_H = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "uniform vec3 colour;\n"
            "void main()\n"
            "{\n"
            "   FragColor = vec4(colour, 1.0f);\n"
            "}\0";

        unsigned int shaderProgram = 0;         // Initialised to 0 to check are being corretcly assigned -> Was getting seg.faults due to this + window context
        unsigned int vertexShaderID = 0;    // ### THESE GET DELETED ANYWAY SO CAN PROB JUST BE LOCAL TO setup() ###
        unsigned int fragmentShaderID = 0;  // ###
        unsigned int VBO = 0;
        unsigned int VAO = 0;
        unsigned int EBO = 0;

        // ######
        // ### CHANGE TO DYNAMIC DRAW IN SETUP AFTER THE STATIC TEST
        // ######
        float vertex_data[3 +DATA_LIMIT_MAX];
        int vertex_indices[4 +2*(DATA_LIMIT_MAX-1)];
};

#endif
