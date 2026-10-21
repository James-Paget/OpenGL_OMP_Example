#include <iostream>
#include "../glad.h"
#include "ge_bar.h"

GE_Bar::GE_Bar(glm::vec3 origin, glm::vec3 dimensions) : Graphical_Element(origin, dimensions) {
    //pass
}
GE_Bar::~GE_Bar() {
    //pass
}
void GE_Bar::setup_full() {
    /*
    . Sets up the shader, VAO, VBO, EBO and attribute assignment for this graphical element
    . Only needs to be called once on creation, then you can just bind the VAO later when wanting to draw this object
    */
    setup_shader();
    setup_refresh();
}
void GE_Bar::setup_shader() {
    unsigned int vertexShaderID;
    unsigned int fragmentShaderID;
    shaderProgram = glCreateProgram();
    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &vertex_shader_GE_BAR_H, NULL);
    glCompileShader(vertexShaderID);
    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragment_shader_GE_BAR_H, NULL);
    glCompileShader(fragmentShaderID);

    glAttachShader(shaderProgram, vertexShaderID);
    glAttachShader(shaderProgram, fragmentShaderID);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
}
void GE_Bar::setup_refresh() {
    /*
    . Freshes the vertex/index data after data is changed in the bar
    . This should be re-called whenever bar_values are added/removed/adjusted
    */
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
void GE_Bar::generate_vertex_info() {
    /*
    . Rebuilds the vertex/index data for this graphical element based on the bar_values list
    */
    float value_complete_total = 0.0f;   // So each can be expressed as a fraction of the bar
    for(int i=0; i<number_of_values; i++) { value_complete_total += bar_values[i]; }

    const float value_step = (value_complete_total>0.0f) ? dimensions.y/value_complete_total : 0.00001f;  // Screen distance moved per unit of value
    float value_running_total = 0.0f;

    vertex_data[0] = origin.x -0.5f*dimensions.x;    // Top-left vertex (FULL BAR)
    vertex_data[1] = origin.y +0.5f*dimensions.y;
    vertex_data[2] = 0.0f;
    vertex_data[3] = origin.x +0.5f*dimensions.x;    // Top-right vertex (FULL BAR)
    vertex_data[4] = origin.y +0.5f*dimensions.y;
    vertex_data[5] = 0.0f;

    for(int i=0; i<number_of_values-1; i++) {
        value_running_total += bar_values[i];
        vertex_data[6 +6*i+0] = origin.x -0.5f*dimensions.x;    // Left-wall vertex
        vertex_data[6 +6*i+1] = origin.y +0.5f*dimensions.y -value_running_total*value_step;
        vertex_data[6 +6*i+2] = 0.0f;
        vertex_data[6 +6*i+3] = origin.x +0.5f*dimensions.x;    // Right-wall vertex
        vertex_data[6 +6*i+4] = origin.y +0.5f*dimensions.y -value_running_total*value_step;
        vertex_data[6 +6*i+5] = 0.0f;

        vertex_indices[0 +i*6] = 2*i;       // Top-left tri
        vertex_indices[1 +i*6] = 2*i +1;
        vertex_indices[2 +i*6] = 2*i +2;
        vertex_indices[3 +i*6] = 2*i +2;    // Bottom-right tri
        vertex_indices[4 +i*6] = 2*i +3;
        vertex_indices[5 +i*6] = 2*i +1;
    }

    vertex_indices[0 +(number_of_values-1)*6] = 2*(number_of_values-1);       // Top-left tri (final segment)
    vertex_indices[1 +(number_of_values-1)*6] = 2*(number_of_values-1) +1;
    vertex_indices[2 +(number_of_values-1)*6] = 2*(number_of_values-1) +2;
    vertex_indices[3 +(number_of_values-1)*6] = 2*(number_of_values-1) +2;    // Bottom-right tri (final segment)
    vertex_indices[4 +(number_of_values-1)*6] = 2*(number_of_values-1) +3;
    vertex_indices[5 +(number_of_values-1)*6] = 2*(number_of_values-1) +1;
    
    vertex_data[0 +3*2*(number_of_values-1) +6] = origin.x -0.5f*dimensions.x;    // Bottom-left vertex (FULL BAR)
    vertex_data[1 +3*2*(number_of_values-1) +6] = origin.y -0.5f*dimensions.y;
    vertex_data[2 +3*2*(number_of_values-1) +6] = 0.0f;
    vertex_data[3 +3*2*(number_of_values-1) +6] = origin.x +0.5f*dimensions.x;    // Bottom-right vertex (FULL BAR)
    vertex_data[4 +3*2*(number_of_values-1) +6] = origin.y -0.5f*dimensions.y;
    vertex_data[5 +3*2*(number_of_values-1) +6] = 0.0f;

    // ### Test-Data ###
    // vertex_data[0] = -0.25f;vertex_data[1] =  0.25f;vertex_data[2] = 0.0f;
    // vertex_data[3] =  0.25f;vertex_data[4] =  0.25f;vertex_data[5] = 0.0f;
    // vertex_data[6] =  -0.25f;vertex_data[7] = -0.25f;vertex_data[8] = 0.0f;
    // vertex_data[9] =  0.25f;vertex_data[10] = -0.25f;vertex_data[11] = 0.0f;
    // vertex_indices[0] = 0;vertex_indices[1] = 1;vertex_indices[2] = 2;
    // vertex_indices[3] = 2;vertex_indices[4] = 3;vertex_indices[5] = 1;
    // ### Test-Data ###
}
void GE_Bar::assign_shader() {
    glUseProgram(shaderProgram);    // Applies its own shader to current context -> may want to use another shader? (generic held by HUD instead?)
}
void GE_Bar::bind_VAO() {
    glBindVertexArray(VAO);
}
void GE_Bar::render() {
    for(int i=0; i<number_of_values; i++) {
        glm::vec3 sectorColour = glm::vec3(1.0f*sin(i*3.14f*0.1f), 1.0f*cos(i*3.14f*0.333f), 0.0f);
        glUniform3fv( glGetUniformLocation(shaderProgram, "sectorColour"), 1, glm::value_ptr(sectorColour) );
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6*i*sizeof(float)));   // [Vertex points across all tris total, ..., offset]
    }
}

void GE_Bar::refresh_data(float *input_bar_values, int length) {
    /*
    . Takes in N floats as weights for each segment of the bar
    */
    for(int i=0; i<std::min(int(VALUE_MAXIMUM), length); i++) {
        bar_values[i] = input_bar_values[i];
    }
    number_of_values = std::min(int(VALUE_MAXIMUM), length);
    generate_vertex_info();
    setup_refresh();
}

void GE_Bar::print_vertex_info() {
    std::cout << "VERTEX_DATA = " << std::endl;
    for(int i=0; i<(VALUE_MAXIMUM+1)*6; i++) {
        std::cout << vertex_data[i] << std::endl;
    }
    std::cout << "VERTEX_INDICES = " << std::endl;
    for(int i=0; i<VALUE_MAXIMUM*6; i++) {
        std::cout << vertex_indices[i] << std::endl;
    }
}