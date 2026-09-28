#include <omp.h>
// #include <mpi.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "custom_multi_example.h"

const char *vertexShaderSourceMulti = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColour;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "out vec3 colourData;\n"
    "void main()\n"
    "{\n"
    "   colourData = aColour;\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "}\0";

const char *fragmentShaderSourceMulti = "#version 330 core\n"
    "in vec3 colourData;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(colourData, 1.0f);\n"
    "}\0";

void perform_custom_multi_example_A(GLFWwindow* window, const unsigned int screen_width, const unsigned int screen_height) {
    #pragma omp parallel
    {
        int thread_index = omp_get_thread_num();
        std::cout << "Thread " << thread_index << " Being Used" << std::endl;
    }
    
    glEnable(GL_DEPTH_TEST);

    float camera_rotation_theta = 65.0f;
    float camera_rotation_phi = 0.0f;
    float fov = 45.0f;

    float vertex_data[] = {    // +Colour data
        0.25f, 0.0f, -0.5f, 1.0f, 0.0f, 0.0f,    // 4 points on base of prism; CW naming from top-down
        -0.25f, 0.0f, -0.5f, 1.0f, 0.0f, 0.0f,
        -0.25f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.25f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f,
        -0.15f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,     // 2 points for the 'ridge' of the prism; Left to Right from top-down
        0.15f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };
    unsigned int indices[] = {  // 3 square faces + 2 triangle faces => 3*2 +2=8 triangles total
        0,1,2,      // 2 for base of the prism
        2,3,0,      //
        0,5,4,      // 2 for the back square face
        4,1,0,      //
        2,4,5,      // 2 for the front square face
        2,3,5,      //
        1,2,4,      // Left triangular face
        0,3,5       // Right triangular face
    };

    glm::vec3 prism_positions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.8f ,0.0f ,0.0f),
        glm::vec3(-0.5f,0.0f, 0.0f)
    };
    glm::vec3 prism_rotations[] = {     // In degrees about X,Y,Z axes
        glm::vec3(45.0f, 45.0f, 0.0f),
        glm::vec3(0.0f, 45.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 45.0f)
    };

    // Setup shader
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    unsigned int vertexShaderID;
    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &vertexShaderSourceMulti, NULL);
    glCompileShader(vertexShaderID);
    unsigned int fragmentShaderID;
    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragmentShaderSourceMulti, NULL);
    glCompileShader(fragmentShaderID);

    glAttachShader(shaderProgram, vertexShaderID);
    glAttachShader(shaderProgram, fragmentShaderID);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    // Make VBO, VAO, EBO
    unsigned int VBO_prism;     // Vertex array buffer for a prism (contains vertices needed for this shader + attributes attached) -> Shape can then be made N times at N separate locations using this one defintion of a prism
    unsigned int VAO_prism;     // Vertex array buffer for a prism (contains vertices needed for this shader + attributes attached) -> Shape can then be made N times at N separate locations using this one defintion of a prism
    unsigned int EBO_prism;     // Specifying shape through indices, hence EBO needed too (not just a VBO)
    
    glGenBuffers(1, &VBO_prism);   // Generate 1 buffer, give ID to VAO_prism
    glBindBuffer(GL_ARRAY_BUFFER, VBO_prism);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);   // Vertex info will be static hence STATIC_DRAW -> If the shape changed (NOT Just transformed, then DYNAMIC_DRAW needed) 
    
    glGenVertexArrays(1, &VAO_prism);   // Generate 1 buffer, give ID to VAO_prism
    glBindVertexArray(VAO_prism);

    glGenBuffers(1, &EBO_prism);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_prism);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);    // This object ONLY interested in indices -> Vertex data pulled from bound VBO

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0); // [layout pos 0, each 3 long, of float, no ints, stride of 3 floats, 0 offset (0th element in stride)] Now the buffers are setup, do a one-time attribute assignment for vertices used in VBO (which is used in the EBO and then VAO) which will be remembered by the VAO later
    glEnableVertexAttribArray(0);   // Assigns this attribute to VAO_prism vertices (VAO already bound so ready to go -> JUST need to bindVertexArray() each time to update it, which handles rebinding VBOs, attributes, etc)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float))); // 0=Pos, 1=Colour
    glEnableVertexAttribArray(1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Fill the face of drawn VAOs, not wireframe, and colour the backs too

    while(!glfwWindowShouldClose(window)) {     // Render while window is open
        calculateKeyPresses(window, &camera_rotation_theta, &camera_rotation_phi, &fov, &(prism_rotations[0].x));

        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);    // Uses the shader compiled
        glBindVertexArray(VAO_prism);   // Updates/resets VAO_prism for next frame
        for(uint i=0; i<3; i++) {   // For each prism, draw separately
            glm::mat4 model = glm::mat4(1.0f);          // Init identity matrices
            glm::mat4 view = glm::mat4(1.0f);           //
            glm::mat4 projection = glm::mat4(1.0f);     //

            // Apply transforms to meshes
            model = glm::rotate(model, glm::radians(prism_rotations[i].x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(prism_rotations[i].y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(prism_rotations[i].z), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::translate(model, prism_positions[i]);

            // Apply transforms to 'camera'
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));
            view = glm::rotate(view, glm::radians(camera_rotation_phi), glm::vec3(1.0f, 0.0f, 0.0f));
            view = glm::rotate(view, glm::radians(camera_rotation_theta), glm::vec3(0.0f, 1.0f, 0.0f));
            projection = glm::perspective(glm::radians(fov), (float)screen_width / (float)screen_height, 0.1f, 100.0f);

            // Set uniforms for each prism
            glUniformMatrix4fv( glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model) );
            glUniformMatrix4fv( glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view) );
            glUniformMatrix4fv( glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection) );

            // Draws the individual prism
            glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0); // [Draw type (indices in 3s of tris => triangle draw), how many vertices TOTAL (doubles included) to draw (all N normally => length/3), what data type of indices (+ve int == unsigned int), offset to indices to start at]
        }

        glfwSwapBuffers(window);    // ### CHECK WHAT ROLES THESE ARE DOING ###
        glfwPollEvents();           // ########################################
    }
    glDeleteVertexArrays(1, &VAO_prism);
    glDeleteVertexArrays(1, &VBO_prism);
}

void calculateKeyPresses(GLFWwindow *window, float *camera_rotation_theta, float *camera_rotation_phi, float *fov, float *rot_value) {
    /*
    . Reacts to key-presses within the given window
    */
    if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        std::cout << "Pressed 1" << std::endl;
        *camera_rotation_theta += 0.4;
    }
    if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        std::cout << "Pressed 2" << std::endl;
        *camera_rotation_theta -= 0.4;
    }
    if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        std::cout << "Pressed 3" << std::endl;
        *camera_rotation_phi += 0.4;
    }
    if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        std::cout << "Pressed 4" << std::endl;
        *camera_rotation_phi -= 0.4;
    }
    if(glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
        std::cout << "Pressed 5" << std::endl;
        *fov += 0.4;
    }
    if(glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
        std::cout << "Pressed 6" << std::endl;
        *fov -= 0.4;
    }
    if(glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
        std::cout << "Pressed 7" << std::endl;
        *rot_value += 0.4;
    }
    if(glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
        std::cout << "Pressed 8" << std::endl;
        *rot_value -= 0.4;
    }
}

/*
. * Try different meshes loaded
. * Try textures

. * Mix multi-threading with OpenGL calls

. * Try lighting
. * Try importing meshes
-- DT cpp+processing
*/

