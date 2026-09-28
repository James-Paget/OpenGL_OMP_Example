#include<iostream>
#include "glad.h"
#include<GLFW/glfw3.h>

void perform_custom_multi_example_A(GLFWwindow* window, const unsigned int screen_width, const unsigned int screen_height);
void calculateKeyPresses(GLFWwindow *window, float *camera_rotation_theta, float *camera_rotation_phi, float *fov, float *rot_value);