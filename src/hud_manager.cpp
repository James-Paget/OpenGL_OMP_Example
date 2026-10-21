#include <iostream>
#include "glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "hud_manager.h"

HUD_Manager::HUD_Manager() {
    //pass
}
HUD_Manager::~HUD_Manager() {
    for(int i=0; i<sizeof(graphical_elements)/sizeof(graphical_elements[0]); i++) {
        delete graphical_elements[i];
    }
}

void HUD_Manager::setup() {
    graphical_elements[0] = new GE_Bar( glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3(0.2f, 0.9f, 0.0f) );
    graphical_elements[1] = new GE_Bar( glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(0.2f, 0.6f, 0.0f) );
    graphical_elements[2] = new GE_Linegraph( 
        glm::vec3(-0.2f, 0.0f, 0.0f), 
        glm::vec3(0.4f, 0.75f, 0.0f),
        0.0f, 10.0f,    // X scale (start,end)
        0.0f, 10.0f     // Y scale (start,end)
    );

    // for(int i=0; i<sizeof(graphical_elements)/sizeof(graphical_elements[0]); i++) {
    for(int i=0; i<(sizeof(graphical_elements)/sizeof(graphical_elements[0])); i++) {
        graphical_elements[i]->setup_full();
    }
}
void HUD_Manager::render(GLFWwindow *window) {
    glfwMakeContextCurrent(window);
    glClearColor(0.0f, 0.0f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(int i=0; i<sizeof(graphical_elements)/sizeof(graphical_elements[0]); i++) {
        graphical_elements[i]->assign_shader();
        graphical_elements[i]->bind_VAO();
        graphical_elements[i]->render();
    }

    glfwSwapBuffers(window);
}