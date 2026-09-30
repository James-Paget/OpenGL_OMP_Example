#include <iostream>
#include <ctime>
#include "glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "manager.h"


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


void manage_simulation_main(GLFWwindow *core_window, GLFWwindow *hud_window, unsigned int screen_width, const unsigned int screen_height) {
    /*
    . Defines the behaviour of the simulation logic and visuals
    . Separated from the core initialisation of the OpenGL
    */
    // Setup data structures
    const unsigned int creature_number = 100;
    Creature creatures[creature_number];

    const unsigned int resource_number = 10;
    Resource resources[resource_number];

    float entity_vertex_data[] = {
        // Creature Cube
        0.25f, 0.25f, 0.25f, 1.0f, 0.0f, 0.0f,    // Top Layer (CCW, Bottom-Right Start) -> RED
        0.25f, 0.25f, -0.25f, 1.0f, 0.0f, 0.0f,
        -0.25f, 0.25f, -0.25f, 1.0f, 0.0f, 0.0f,
        -0.25f, 0.25f, 0.25f, 1.0f, 0.0f, 0.0f,
        0.25f, -0.25f, 0.25f, 0.0f, 0.0f, 1.0f,    // Bottom Layer (CCW, Bottom-Right Start) -> BLUE
        0.25f, -0.25f, -0.25f, 0.0f, 0.0f, 1.0f,
        -0.25f, -0.25f, -0.25f, 0.0f, 0.0f, 1.0f,
        -0.25f, -0.25f, 0.25f, 0.0f, 0.0f, 1.0f,
        // Resource Diamond
        0.15f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,    // 4 Planar vertices
        0.0f, 0.0f, -0.15f, 0.0f, 0.0f, 1.0f,
        -0.15f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.15f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.15f, 0.0f, 1.0f, 1.0f, 1.0f,    // Top
        0.0f, -0.15f, 0.0f, 1.0f, 1.0f, 1.0f,   // Bottom
    };
    int entity_vertex_indices[] = {
        // Creature Cube
        0,1,2,  // Top-face
        2,3,0,
        0,4,1,  // Right-face
        1,5,4,
        4,0,3,  // Front-face
        3,7,4,
        4,7,6,  // Bottom-face
        6,4,5,
        5,6,1,  // Back-face
        1,2,6,
        6,2,3,  // Left-face
        3,6,7,
        // Resource Diamond
        8,12,9,     // Upper faces
        9,12,10,
        10,12,11,
        11,12,8,
        8,13,9,    // Lower faces
        9,13,10,
        10,13,11,
        11,13,8,
    };

    // Populate data structures made
    // float spawn_dimensions[3] = {float(screen_width), float(screen_height), float(screen_height)};
    float spawn_dimensions[3] = {7.5f, 7.5f, 7.5f};
    populate_creatures(creatures, creature_number, spawn_dimensions);
    populate_resources(resources, resource_number, spawn_dimensions);

    // Setup shaders + OpenGL arrays
    glEnable(GL_DEPTH_TEST);

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
    // ###
    // ###
    // ### MULTIPLE VAOs/VBOs/EBOs
    // ### OR 
    // ### WITHIN A SINGLE BUFFER SHUFFLE BY SOME OFFSET
    // ###
    // ###
    unsigned int VBO_entity;     // Vertex array buffer for a prism (contains vertices needed for this shader + attributes attached) -> Shape can then be made N times at N separate locations using this one defintion of a prism
    unsigned int VAO_entity;     // Vertex array buffer for a prism (contains vertices needed for this shader + attributes attached) -> Shape can then be made N times at N separate locations using this one defintion of a prism
    unsigned int EBO_entity;     // Specifying shape through indices, hence EBO needed too (not just a VBO)
    
    glGenBuffers(1, &VBO_entity);   // Generate 1 buffer, give ID to VAO_entity
    glBindBuffer(GL_ARRAY_BUFFER, VBO_entity);
    glBufferData(GL_ARRAY_BUFFER, sizeof(entity_vertex_data), entity_vertex_data, GL_STATIC_DRAW);   // Vertex info will be static hence STATIC_DRAW -> If the shape changed (NOT Just transformed, then DYNAMIC_DRAW needed) 
    
    glGenVertexArrays(1, &VAO_entity);   // Generate 1 buffer, give ID to VAO_entity
    glBindVertexArray(VAO_entity);

    glGenBuffers(1, &EBO_entity);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_entity);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(entity_vertex_indices), entity_vertex_indices, GL_STATIC_DRAW);    // This object ONLY interested in indices -> Vertex data pulled from bound VBO

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0); // [layout pos 0, each 3 long, of float, no ints, stride of 3 floats, 0 offset (0th element in stride)] Now the buffers are setup, do a one-time attribute assignment for vertices used in VBO (which is used in the EBO and then VAO) which will be remembered by the VAO later
    glEnableVertexAttribArray(0);   // Assigns this attribute to VAO_entity vertices (VAO already bound so ready to go -> JUST need to bindVertexArray() each time to update it, which handles rebinding VBOs, attributes, etc)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float))); // 0=Pos, 1=Colour
    glEnableVertexAttribArray(1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Fill the face of drawn VAOs, not wireframe, and colour the backs too

    // Render
    float prior_t = glfwGetTime();   // Time on previous frame
    float delta_t = 0.016f;   // Time between rendered frames -> ~0.016 for 60fps (UPDATED every frame anyway, initialised as this to prevent delta_t=0.0f errors)
    while( !glfwWindowShouldClose(core_window) && !glfwWindowShouldClose(hud_window) ) {
        render_core(core_window, shaderProgram, VAO_entity, creatures, creature_number, resources, resource_number, screen_width, screen_height, &prior_t, &delta_t);
        render_hud(hud_window, shaderProgram, VAO_entity, creatures, creature_number, screen_width, screen_height);
        // Once-per frame
        glfwPollEvents();
    }

    // Clean-up
    glDeleteVertexArrays(1, &VAO_entity);
    glDeleteVertexArrays(1, &VBO_entity);

}

void render_core(GLFWwindow *core_window, unsigned int shaderProgram, unsigned int VAO_entity, Creature *creatures, const unsigned int creature_number, Resource *resources, const unsigned int resource_number, const unsigned int screen_width, const unsigned int screen_height, float *prior_t, float *delta_t) {
    // Render CORE
    glfwMakeContextCurrent(core_window);

    *delta_t = (glfwGetTime()-*prior_t +0.0000001f);    // **NOTE; +0.0000001f included to prevent /0 errors
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);    // Uses the shader compiled
    glBindVertexArray(VAO_entity);   // Updates/resets VAO_entity for next frame

    glm::mat4 view = glm::mat4(1.0f);           // Init identity matrices
    glm::mat4 projection = glm::mat4(1.0f);     //
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -25.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)screen_width / (float)screen_height, 0.1f, 100.0f);
    glUniformMatrix4fv( glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view) );
    glUniformMatrix4fv( glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection) );

    for(uint i=0; i<creature_number; i++) {
        // Update physics for each entity
        creatures[i].directional_thrust(creatures[i].forward_direction, 0.005f);
        float dist = std::max<float>( sqrt( pow(creatures[i].position.x, 2) + pow(creatures[i].position.y, 2) + pow(creatures[i].position.z, 2) ), 0.0000001f);
        creatures[i].directional_thrust(
            glm::vec3(
                -creatures[i].position.x/dist,
                -creatures[i].position.y/dist,
                -creatures[i].position.z/dist
            ), 
            0.01f
        );
        creatures[i].update_dynamics(*delta_t);

        // Draw each entity
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, creatures[i].position);

        // Set uniforms for each entity
        glUniformMatrix4fv( glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model) );

        // Draws the individual entities -> First 36, 0 offset
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    for(uint i=0; i<resource_number; i++) {
        // Draw each resource
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, resources[i].position);

        // Set uniforms for each entity
        glUniformMatrix4fv( glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model) );

        // Draws the individual entities -> Next 24 tris, 36 offset
        glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, (void*)(36*sizeof(float)));
    }

    *prior_t = glfwGetTime();
    glfwSwapBuffers(core_window);
}
void render_hud(GLFWwindow *hud_window, unsigned int shaderProgram, unsigned int VAO_entity, Creature *creatures, const unsigned int creature_number, const unsigned int screen_width, const unsigned int screen_height) {
    // Render HUD
    glfwMakeContextCurrent(hud_window);
    glClearColor(float(creature_number)/100.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);    // Uses the shader compiled
    glBindVertexArray(VAO_entity);   // Updates/resets VAO_entity for next frame
    glfwSwapBuffers(hud_window);
}

void populate_creatures(Creature *creatures, const unsigned int creature_number, float *spawn_dimemsions) {
    /*
    . Goes through each empty entity in a list and populates it with randomised starting data

    . creatures = list of creatures
    . spawn_dimenions = [X,Y,Z] widths for spawnable half-width (+- allowed)
    */
    for(int i=0; i<creature_number; i++) {
        Creature newCreature = Creature();

        newCreature.creature_type = 1;

        newCreature.id = i;
        newCreature.position = glm::vec3(
            spawn_dimemsions[0]*(0.5f -rand_float(i*creature_number+1))*2,
            spawn_dimemsions[1]*(0.5f -rand_float(i*creature_number+2))*2,
            spawn_dimemsions[2]*(0.5f -rand_float(i*creature_number+3))*2
        );
        float dir_theta = 2.0f*3.14f*rand_float(i*creature_number +4);
        float dir_phi = 1.0f*3.14f*rand_float(i*creature_number +5);
        newCreature.forward_direction = glm::vec3(cos(dir_theta)*sin(dir_phi), sin(dir_theta)*sin(dir_phi), cos(dir_phi));
        newCreature.up_direction = glm::vec3(cos(dir_theta)*sin(dir_phi-3.14f/2.0f), sin(dir_theta)*sin(dir_phi-3.14f/2.0f), cos(dir_phi-3.14f/2.0f));
        creatures[i] = newCreature;
    }
}
void populate_resources(Resource *resources, const unsigned int resource_number, float *spawn_dimemsions) {
    /*
    . Goes through each empty entity in a list and populates it with randomised starting data

    . resources = list of resources
    . spawn_dimenions = [X,Y,Z] widths for spawnable half-width (+- allowed)
    */
    for(int i=0; i<resource_number; i++) {
        Resource newResource = Resource();

        newResource.resource_type = 1;

        newResource.id = i;
        newResource.position = glm::vec3(
            spawn_dimemsions[0]*(0.5f -rand_float(i*resource_number+1))*2,
            spawn_dimemsions[1]*(0.5f -rand_float(i*resource_number+2))*2,
            spawn_dimemsions[2]*(0.5f -rand_float(i*resource_number+3))*2
        );
        float dir_theta = 2.0f*3.14f*rand_float(i*resource_number +4);
        float dir_phi = 1.0f*3.14f*rand_float(i*resource_number +5);
        newResource.forward_direction = glm::vec3(cos(dir_theta)*sin(dir_phi), sin(dir_theta)*sin(dir_phi), cos(dir_phi));
        newResource.up_direction = glm::vec3(cos(dir_theta)*sin(dir_phi-3.14f/2.0f), sin(dir_theta)*sin(dir_phi-3.14f/2.0f), cos(dir_phi-3.14f/2.0f));
        resources[i] = newResource;
    }
}

float rand_float(int seed_offset) {
    /*
    . Seeds and returns a random float in range (0,1)
    ######
    ### NEED TO SEED DIFFERENTLY WITHIN THE SAME FRAME
    ######
    */
    std::srand(std::time(0)+seed_offset);
    return (float)(std::rand()>>23)/256.0f;
}
