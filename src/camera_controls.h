#include "GLFW/glfw3.h"

#ifndef CAMERA_CONTROLS_H
#define CAMERA_CONTROLS_H

void calculate_camera_controls(GLFWwindow *window, glm::vec3 *camera_origin, float *camera_theta, float *camera_phi, float *camera_fov);

#endif