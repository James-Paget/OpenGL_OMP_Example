#include <iostream>
#include "../glad.h"
#include "ge_bar.h"

GE_Bar::GE_Bar(glm::vec3 origin, glm::vec3 dimensions) : Graphical_Element(origin, dimensions) {
    //pass
}
void GE_Bar::setup() {
    /*
    . Sets up the shader, VAO, VBO, EBO and attribute assignment for this graphical element
    . Only needs to be called once on creation, then you can just bind the VAO later when wanting to draw this object
    */
    std::cout << "  GE_BAR setup start..." << std::endl;
    // Setup shaders
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

    // Setup buffers
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW); 
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0); // [layout pos 0, each 3 long, of float, no ints, stride of 3 floats, 0 offset (0th element in stride)] Now the buffers are setup, do a one-time attribute assignment for vertices used in VBO (which is used in the EBO and then VAO) which will be remembered by the VAO later
    glEnableVertexAttribArray(0);
    std::cout << "  GE_BAR setup complete!" << std::endl;
}
void GE_Bar::assign_shader() {
    glUseProgram(shaderProgram);    // Applies its own shader to current context -> may want to use another shader? (generic held by HUD instead?)
}
void GE_Bar::bind_VAO() {
    glBindVertexArray(VAO);
}
void GE_Bar::render() {
    std::cout << "shaderProgram = " << shaderProgram << std::endl;
    std::cout << "vertexShaderID = " << vertexShaderID << std::endl;
    std::cout << "fragmentShaderID = " << fragmentShaderID << std::endl;
    std::cout << "VBO = " << VBO << std::endl;
    std::cout << "VAO = " << VAO << std::endl;
    std::cout << "EBO = " << EBO << std::endl;
    // ###
    // ### DO ALL THE SETUP IN THE RIGHT CONTEXT
    // ###
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);   // [Vertex points across all tris total, ..., offset]
}