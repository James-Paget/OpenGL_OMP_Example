#include <iostream>
#include "../glad.h"
#include "ge_linegraph.h"

GE_Linegraph::GE_Linegraph(glm::vec3 origin, glm::vec3 dimensions, float x_scale_start, float x_scale_end, float y_scale_start, float y_scale_end) : Graphical_Element(origin, dimensions) {
    this->x_scale_start = x_scale_start;
    this->x_scale_end   = x_scale_end;
    this->y_scale_start = y_scale_start;
    this->y_scale_end   = y_scale_end;
}
GE_Linegraph::~GE_Linegraph() {
    //pass
}

void GE_Linegraph::setup_full() {
    setup_shader();
    setup_refresh();
}
void GE_Linegraph::setup_shader() {
    unsigned int vertexShaderID;
    unsigned int fragmentShaderID;
    shaderProgram = glCreateProgram();
    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &vertex_shader_GE_LINEGRAPH_H, NULL);
    glCompileShader(vertexShaderID);
    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragment_shader_GE_LINEGRAPH_H, NULL);
    glCompileShader(fragmentShaderID);

    glAttachShader(shaderProgram, vertexShaderID);
    glAttachShader(shaderProgram, fragmentShaderID);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
}
void GE_Linegraph::setup_refresh() {
    // Setup buffers
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_DYNAMIC_DRAW); 
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0); // [layout pos 0, each 3 long, of float, no ints, stride of 3 floats, 0 offset (0th element in stride)] Now the buffers are setup, do a one-time attribute assignment for vertices used in VBO (which is used in the EBO and then VAO) which will be remembered by the VAO later
    glEnableVertexAttribArray(0);
}
void GE_Linegraph::generate_vertex_info() {
    /*
    . Generates line vertex + index data
    . Formatted as:
        - Axes lines
        - Line A data
        - Line B data
        - ... ETC ...
    */
    // Axes
    vertex_data[0] = origin.x;              vertex_data[1] = origin.y;               vertex_data[2] = 0.0f;
    vertex_data[3] = origin.x;              vertex_data[4] = origin.y +dimensions.y; vertex_data[5] = 0.0f;
    vertex_data[6] = origin.x+dimensions.x; vertex_data[7] = origin.y;               vertex_data[8] = 0.0f;

    vertex_indices[0] = 0; vertex_indices[1] = 1; vertex_indices[2] = 0; vertex_indices[3] = 2;

    // Line
    // ###
    // ### Could sort, then display, but for now this is fine
    // ###
    if(data_x.size() == data_y.size()) {  // Preliminary check to make sure data has been added correctly
        for(int i=0; i<data_x.size(); i++) {
            // ###
            // ### CHECK INBOUNDS OF FIGURE
            // ###
            // ### CHECK IN BOUNDS OF VERTEX LIMIT
            // ###
            vertex_data[9 +3*i   ] = origin.x +(data_x[i]-x_scale_start)*(dimensions.x/(x_scale_end-x_scale_start));
            vertex_data[9 +3*i +1] = origin.y +(data_y[i]-y_scale_start)*(dimensions.y/(y_scale_end-y_scale_start));
            vertex_data[9 +3*i +2] = 0.0f;

            if(i > 0) { // Connects to n-1th data point, hence skip 0th
                vertex_indices[3 +2*i -1] = i+2;
                vertex_indices[3 +2*i   ] = i+3;
            }
        }
    }
}
void GE_Linegraph::assign_shader() {
    glUseProgram(shaderProgram);
}
void GE_Linegraph::bind_VAO() {
    glBindVertexArray(VAO);
}
void GE_Linegraph::render() {
    // Axes
    glm::vec3 colour = glm::vec3(1.0f, 1.0f, 1.0f);
    glUniform3fv( glGetUniformLocation(shaderProgram, "colour"), 1, glm::value_ptr(colour) );
    glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, (void*)(0));   // [Vertex points across all tris total, ..., offset]
    // Lines
    colour = glm::vec3(1.0f, 0.0f, 0.0f);
    glUniform3fv( glGetUniformLocation(shaderProgram, "colour"), 1, glm::value_ptr(colour) );
    glDrawElements(GL_LINES, 2*(data_x.size()-1), GL_UNSIGNED_INT, (void*)(4*sizeof(float)));   // [Vertex points across all tris total, ..., offset]
}

void GE_Linegraph::refresh_data(float *input_data_x, float *input_data_y, int length) {
    /*
    . Takes X,Y data and copies this to its own buffers
    */
    //pass
    generate_vertex_info();
    setup_refresh();
}