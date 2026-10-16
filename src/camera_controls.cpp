#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "camera_controls.h"
#include "simulation_consts.h"

void calculate_camera_controls(GLFWwindow *window, glm::vec3 *camera_origin, float *camera_theta, float *camera_phi, float *camera_fov) {
    /*
    . Manages all the controls for translating/rotating the camera
    */
    if(GLFW_PRESS) {
        // ###
        // ### MAYBE SWAP PHI UP ALIGNMENT TO Y NOT Z
        // ###      -> Will have to change this for the rest of the program too
        // ###
        glm::vec3 camera_forward = glm::vec3(
            cos(*camera_theta)*sin(*camera_phi),
            sin(*camera_theta)*sin(*camera_phi),
            cos(*camera_phi)
        );
        // Translation controls
        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { *camera_origin = *camera_origin +camera_forward+CAMERA_MOVE_SPEED; }
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { *camera_origin = *camera_origin -camera_forward+CAMERA_MOVE_SPEED; }
        // NOTE** Up manually defined as (0,1,0) always here
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { *camera_origin = *camera_origin +glm::cross(camera_forward, glm::vec3(0.0f, 1.0f, 0.0f))+CAMERA_MOVE_SPEED; }
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { *camera_origin = *camera_origin -glm::cross(camera_forward, glm::vec3(0.0f, 1.0f, 0.0f))+CAMERA_MOVE_SPEED; }
        if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { *camera_origin = *camera_origin +glm::vec3(0.0f, 1.0f, 0.0f)+CAMERA_MOVE_SPEED; }
        if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { *camera_origin = *camera_origin -glm::vec3(0.0f, 1.0f, 0.0f)+CAMERA_MOVE_SPEED; }
        
        // Rotation controls
        if(glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) { *camera_phi = *camera_phi +CAMERA_ROTATE_SPEED; }
        if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) { *camera_phi = *camera_phi -CAMERA_ROTATE_SPEED; }
        if(glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) { *camera_theta = *camera_theta +CAMERA_ROTATE_SPEED; }
        if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) { *camera_theta = *camera_theta -CAMERA_ROTATE_SPEED; }
        
        // FOV controls
        if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) { *camera_fov = *camera_fov -CAMERA_ZOOM_SPEED; }
        if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) { *camera_fov = *camera_fov +CAMERA_ZOOM_SPEED; }
    }
}