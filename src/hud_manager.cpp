#include <iostream>
#include "glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "hud_manager.h"

HUD_Manager::HUD_Manager() {
    //pass
}

void HUD_Manager::setup() {
    // ###
    // ### GO THROUGH EACH GRAPHICAL ELEMENT AND SET IT UP
    // ###
    std::cout << "HUD setup start..." << std::endl;
    element_ge_bar->setup();
    std::cout << "HUD setup complete!" << std::endl;
}
void HUD_Manager::render(GLFWwindow *window) {
    glfwMakeContextCurrent(window);
    glClearColor(0.0f, 0.0f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    element_ge_bar->assign_shader();
    element_ge_bar->bind_VAO();
    
    element_ge_bar->render();
    //pass -> Render each graphical element -> switch the to shaders they want and buffers they want

    glfwSwapBuffers(window);
}