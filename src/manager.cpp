#include <iostream>
#include <ctime>
#include "glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <omp.h>

#include "manager.h"
#include "camera_controls.h"
#include "simulation_consts.h"


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


const char *vertexShader_pingSphere = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "}\0";

const char *fragmentShader_pingSphere = "#version 330 core\n"
    "uniform vec4 colourData;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = colourData;\n"
    "}\0";


const char *vertexShader_terrain= "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "uniform mat4 coefficients;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "out vec4 heightCol;\n"
    "void main()\n"
    "{\n"
    "   float height = 0.0;\n"
    "   for(int i=0; i<16; i++) {\n"
    "       if(i%2==0) {\n"
    "           height += coefficients[int(floor(float(i)/4.0))][i%4]*sin( i*3.14*aPos.x*0.04f )*0.35;\n"
    "       } else {\n"
    "           height += coefficients[int(floor(float(i)/4.0))][i%4]*sin( i*3.14*aPos.y*0.04f )*0.35;\n"
    "       }\n"
    "   }\n"
    "   gl_Position = projection * view * model * vec4(aPos.x, height, aPos.y, 1.0);\n"
    "   heightCol = vec4(0.5 -height*0.25, 0.5 +height*0.25, 0.0, 1.0);\n"
    "}\0";


const char *fragmentShader_terrain = "#version 330 core\n"
    "in vec4 heightCol;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = heightCol;\n"
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

    int ping_number = 0;   // Counts pings as they are added and removed
    const unsigned int pings_maximum = 6*PING_MAXIMUM;    // E.G 6*X length => X pings max
    // Pings stored as {
    //    X_centre, 
    //    Y_centre, 
    //    Z_centre, 
    //    start_time, strength, signal,
    //       ... REPEATING ...
    // } to specify the ping center
    float pings[6*PING_MAXIMUM] = {};   // {} initialisation just to set all values to 0 to start with

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

    float ping_sphere_vertex_data[] = {
        1.0f, 0.0f, 0.0f,      // 4 Planar vertices
        0.0f, 0.0f, -1.0f,
        -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f,      // Top
        0.0f, -1.0f, 0.0f,     // Bottom
    };        // [X,Y,Z, R,G,B]
    int ping_sphere_vertex_indices[] = {
        0,4,1,     // Upper faces
        1,4,2,
        2,4,3,
        3,4,0,
        0,5,1,    // Lower faces
        1,5,2,
        2,5,3,
        3,5,0
    };       // [I1, I2, I3]


    int x_cells = 50;
    int z_cells = 100;
    float cell_width = 0.4f;
    // ######
    // ### THIS NEEDS SOME CONST STORED NUMBER TO WORK WELL -> DOES NOT LIKE A TEMP RETURN -> FAR ENOUGH
    // ######   --> SAVE FUNCTION CALLS, THEN PUT INTO SIZE
    float terrain_vertex_data[ (x_cells+1)*(z_cells+1)*2 ];     // Initialise blank for generation next
    int terrain_vertex_indices[ (x_cells)*(z_cells)*3*2 ];    //
    glm::mat4 terrain_coefficients = glm::mat4(0.0f);
    generate_terrain_vertex_info(x_cells, z_cells, cell_width, terrain_vertex_data, terrain_vertex_indices);
    generate_terrain_coefficients(4, &terrain_coefficients, 0);

    // Populate data structures made
    // float spawn_dimensions[3] = {float(screen_width), float(screen_height), float(screen_height)};
    float spawn_dimensions[3] = {7.5f, 7.5f, 7.5f};
    populate_creatures(creatures, creature_number, spawn_dimensions);
    populate_resources(resources, resource_number, spawn_dimensions);

    // Setup shaders + OpenGL arrays
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // DEFAULT SHADER
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

    // PING SPHERE SHADER
    unsigned int shaderProgram_pingSphere;
    shaderProgram_pingSphere = glCreateProgram();

    unsigned int vertexShaderID_pingSphere;
    vertexShaderID_pingSphere = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID_pingSphere, 1, &vertexShader_pingSphere, NULL);
    glCompileShader(vertexShaderID_pingSphere);
    unsigned int fragmentShaderID_pingSphere;
    fragmentShaderID_pingSphere = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID_pingSphere, 1, &fragmentShader_pingSphere, NULL);
    glCompileShader(fragmentShaderID_pingSphere);

    glAttachShader(shaderProgram_pingSphere, vertexShaderID_pingSphere);
    glAttachShader(shaderProgram_pingSphere, fragmentShaderID_pingSphere);
    glLinkProgram(shaderProgram_pingSphere);

    glDeleteShader(vertexShaderID_pingSphere);
    glDeleteShader(fragmentShaderID_pingSphere);

    // TERRAIN SHADER
    unsigned int shaderProgram_terrain;
    shaderProgram_terrain = glCreateProgram();

    unsigned int vertexShaderID_terrain;
    vertexShaderID_terrain = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID_terrain, 1, &vertexShader_terrain, NULL);
    glCompileShader(vertexShaderID_terrain);
    unsigned int fragmentShaderID_terrain;
    fragmentShaderID_terrain = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID_terrain, 1, &fragmentShader_terrain, NULL);
    glCompileShader(fragmentShaderID_terrain);

    glAttachShader(shaderProgram_terrain, vertexShaderID_terrain);
    glAttachShader(shaderProgram_terrain, fragmentShaderID_terrain);
    glLinkProgram(shaderProgram_terrain);

    glDeleteShader(vertexShaderID_terrain);
    glDeleteShader(fragmentShaderID_terrain);

    // Make + Setup all VBO, VAO, EBO buffers
    // ---- STANDARD MESHES ----
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

    // ---- PING SPHERE MESHES ----
    unsigned int VBO_ping_sphere;     // Vertex buffer object for a prism (contains vertices needed for this shader + attributes attached) -> Shape can then be made N times at N separate locations using this one defintion of a prism
    unsigned int VAO_ping_sphere;     // Vertex array object for a prism (contains vertices needed for this shader + attributes attached) -> Shape can then be made N times at N separate locations using this one defintion of a prism
    unsigned int EBO_ping_sphere;     // Specifying shape through indices, hence EBO needed too (not just a VBO)
    
    glGenBuffers(1, &VBO_ping_sphere);   // Generate 1 buffer, give ID to VAO_entity
    glBindBuffer(GL_ARRAY_BUFFER, VBO_ping_sphere);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ping_sphere_vertex_data), ping_sphere_vertex_data, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &VAO_ping_sphere);   // Generate 1 buffer, give ID to VAO_entity
    glBindVertexArray(VAO_ping_sphere);

    glGenBuffers(1, &EBO_ping_sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_ping_sphere);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ping_sphere_vertex_indices), ping_sphere_vertex_indices, GL_STATIC_DRAW);    // This object ONLY interested in indices -> Vertex data pulled from bound VBO

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // ---- TERRAIN MESHES ----
    unsigned int VBO_terrain;     // Vertex buffer object for a prism (contains vertices needed for this shader + attributes attached) -> Shape can then be made N times at N separate locations using this one defintion of a prism
    unsigned int VAO_terrain;     // Vertex array object for a prism (contains vertices needed for this shader + attributes attached) -> Shape can then be made N times at N separate locations using this one defintion of a prism
    unsigned int EBO_terrain;     // Specifying shape through indices, hence EBO needed too (not just a VBO)
    
    glGenBuffers(1, &VBO_terrain);   // Generate 1 buffer, give ID to VAO_entity
    glBindBuffer(GL_ARRAY_BUFFER, VBO_terrain);
    glBufferData(GL_ARRAY_BUFFER, sizeof(terrain_vertex_data), terrain_vertex_data, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &VAO_terrain);
    glBindVertexArray(VAO_terrain);

    glGenBuffers(1, &EBO_terrain);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_terrain);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(terrain_vertex_indices), terrain_vertex_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Fill the face of drawn VAOs, not wireframe, and colour the backs too

    // Render
    glm::vec3 camera_origin = glm::vec3(0.0f, 0.0f, -25.0f);
    float camera_theta = 0.0f;
    float camera_phi = 0.0f;
    float camera_fov = 45.0f;

    float prior_t = glfwGetTime();   // Time on previous frame
    float delta_t = 0.016f;   // Time between rendered frames -> ~0.016 for 60fps (UPDATED every frame anyway, initialised as this to prevent delta_t=0.0f errors)
    while( !glfwWindowShouldClose(core_window) && !glfwWindowShouldClose(hud_window) ) {
        calculate_entities(pings, &ping_number, ping_sphere_vertex_data, ping_sphere_vertex_indices, terrain_coefficients, cell_width, creatures, creature_number, resources, resource_number, &prior_t);
        render_core(core_window, shaderProgram, shaderProgram_pingSphere, shaderProgram_terrain, VAO_entity, VAO_ping_sphere, VAO_terrain, pings, &ping_number, ping_sphere_vertex_data, ping_sphere_vertex_indices, &x_cells, &z_cells, terrain_coefficients, creatures, creature_number, resources, resource_number, &camera_origin, &camera_theta, &camera_phi, &camera_fov, screen_width, screen_height, &prior_t, &delta_t);
        render_hud(hud_window, shaderProgram, VAO_entity, creatures, creature_number, screen_width, screen_height);
        // Once-per frame
        glfwPollEvents();
    }

    // Clean-up
    glDeleteVertexArrays(1, &VAO_entity);
    glDeleteVertexArrays(1, &VBO_entity);

}

void calculate_entities(float *pings, int *ping_number, float *ping_sphere_vertex_data, int *ping_sphere_vertex_indices, glm::mat4 terrain_coefficients, float terrain_width, Creature *creatures, const unsigned int creature_number, Resource *resources, const unsigned int resource_number, float *prior_t) {
    /*
    . Performs calculations for entities
    . These include sending and processing pings, updating dynamics, etc
    */
    evaluate_pings(pings, ping_number, creatures, creature_number, resources, resource_number, glfwGetTime());
    calculate_terrain_collision(creatures, creature_number, terrain_coefficients, terrain_width);
    remove_pings(pings, ping_number, glfwGetTime());
}

void render_core(GLFWwindow *core_window, unsigned int shaderProgram, unsigned int shaderProgram_pingSphere, unsigned int shaderProgram_terrain, unsigned int VAO_entity, unsigned int VAO_ping_sphere, unsigned int VAO_terrain, float *pings, int *ping_number, float *ping_sphere_vertex_data, int *ping_sphere_vertex_indices, int *terrain_x_cells, int *terrain_z_cells, glm::mat4 terrain_coefficients, Creature *creatures, const unsigned int creature_number, Resource *resources, const unsigned int resource_number, glm::vec3 *camera_origin, float *camera_theta, float *camera_phi, float *camera_fov, const unsigned int screen_width, const unsigned int screen_height, float *prior_t, float *delta_t) {
    // Render CORE
    glfwMakeContextCurrent(core_window);

    calculate_camera_controls(core_window, camera_origin, camera_theta, camera_phi, camera_fov);
    read_keypress_core(core_window, pings, ping_number, creatures);

    *delta_t = (glfwGetTime()-*prior_t +0.0000001f);    // **NOTE; +0.0000001f included to prevent /0 errors
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);    // Uses the shader compiled

    glm::mat4 view = glm::mat4(1.0f);           // Init identity matrices
    view = glm::translate(view, *camera_origin);
    view = glm::rotate(view, *camera_theta, glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::rotate(view, *camera_phi, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 projection = glm::mat4(1.0f);     // "" ""
    projection = glm::perspective(glm::radians(*camera_fov), (float)screen_width / (float)screen_height, 0.1f, 100.0f);

    glUniformMatrix4fv( glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view) );
    glUniformMatrix4fv( glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection) );

    glBindVertexArray(VAO_entity);   // Updates/resets VAO_entity for next frame
    for(uint i=0; i<creature_number; i++) {
        // Update physics for each entity
        creatures[i].directional_thrust(creatures[i].forward_direction, 0.005f);
        float dist = std::max<float>( sqrt( pow(creatures[i].position.x, 2) + pow(creatures[i].position.y, 2) + pow(creatures[i].position.z, 2) ), 0.0000001f);
        // creatures[i].directional_thrust(
        //     glm::vec3(
        //         -creatures[i].position.x/dist,
        //         -creatures[i].position.y/dist,
        //         -creatures[i].position.z/dist
        //     ), 
        //     0.01f
        // );
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

    // Draw pings
    glUseProgram(shaderProgram_pingSphere);    // Uses the shader compiled
    glUniformMatrix4fv( glGetUniformLocation(shaderProgram_pingSphere, "view"), 1, GL_FALSE, glm::value_ptr(view) );
    glUniformMatrix4fv( glGetUniformLocation(shaderProgram_pingSphere, "projection"), 1, GL_FALSE, glm::value_ptr(projection) );
    glBindVertexArray(VAO_ping_sphere);   // Swaps to the other VAO/EBO needed

    for(uint i=0; i<(*ping_number); i++) {
        // Centre each ping
        float radius = fetch_ping_radius(pings, i, glfwGetTime());
        float strength = fetch_ping_signal_strength(pings, i, glfwGetTime());
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(pings[i*6 +0], pings[i*6 +1], pings[i*6 +2]));
        model = glm::scale(model, glm::vec3(radius));    // NOTE** Model vertex coords has a default radius of 1f, hence can just multiply by radius here
        glm::vec4 colourData = (int(pings[i*6 +5])==1) ? glm::vec4(1.0f, 1.0f, 0.0f, strength-PING_SIGNAL_REMOVE_THRESHOLD) : glm::vec4(1.0f, 0.0f, 0.0f, strength-PING_SIGNAL_REMOVE_THRESHOLD);

        // Set uniforms for each entity
        glUniformMatrix4fv( glGetUniformLocation(shaderProgram_pingSphere, "model"), 1, GL_FALSE, glm::value_ptr(model) );
        glUniform4fv( glGetUniformLocation(shaderProgram_pingSphere, "colourData"), 1, glm::value_ptr(colourData) );

        // Draws the individual pings -> Referring to index offset
        glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, (void*)(0)); // NOTE** Offset set to skip older indices
    }

    // Draw terrain
    glUseProgram(shaderProgram_terrain);    // Uses the shader compiled
    glUniformMatrix4fv( glGetUniformLocation(shaderProgram_terrain, "view"), 1, GL_FALSE, glm::value_ptr(view) );
    glUniformMatrix4fv( glGetUniformLocation(shaderProgram_terrain, "projection"), 1, GL_FALSE, glm::value_ptr(projection) );
    glBindVertexArray(VAO_terrain);   // Swaps to the other VAO/EBO needed

    // N threads used; Split threads over the number of rows needing to be rendered
    // const uint RENDER_THREAD_TOTAL = 4;
    // const int cells_per_thread = int(floor(float(*terrain_z_cells)/float(RENDER_THREAD_TOTAL)));
    // #pragma omp parallel num_threads(RENDER_THREAD_TOTAL)
    // {
    //     // ###
    //     // ### WHY IS THE RENDER BREAKING -> ONLY DOING FIRST SECTION??
    //     // ###
    //     // ###
    //     // ### THIS COULD JUST BE QUEUEING UP GPU RENDERING IN PARALLEL, WHICH HAS NO BENEFIT
    //     // ###  --> FINDING NORMALS IN PARALLEL THEN USING THIS IS PROB BETTER
    //     // ###
    //     int thread_index = omp_get_thread_num();
    //     int z_cell_start = thread_index*cells_per_thread;
    //     int z_cell_end = (thread_index<RENDER_THREAD_TOTAL-1) ? (thread_index+1)*cells_per_thread : *terrain_z_cells;
    //     std::cout << "Thread=" << thread_index << ": {" << z_cell_start <<", "<< z_cell_end << "}" <<std::endl;
    //     for(uint k=z_cell_start; k<z_cell_end; k++) {
    //         for(uint i=0; i<*terrain_x_cells; i++) {
    //             glm::mat4 model = glm::mat4(1.0f);

    //             // Set uniforms for each entity
    //             // ###
    //             // ### DOES THIS NEED TO BE UPLOADED EACH TIME? -> CAN YOU DO IT ONCE AND IT CARRY OVER
    //             // ###
    //             glUniformMatrix4fv( glGetUniformLocation(shaderProgram_terrain, "coefficients"), 1, GL_FALSE, glm::value_ptr(terrain_coefficients) );
    //             glUniformMatrix4fv( glGetUniformLocation(shaderProgram_terrain, "model"), 1, GL_FALSE, glm::value_ptr(model) );
    //             // ### ADD ANOTHER UNIFORM FOR THE Z HEIGHT OF THIS PARTICULAR CELL

    //             // Draws the individual pings -> Referring to index offset
    //             int index_offset = 6*(i +k*(*terrain_x_cells));
    //             glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(index_offset*sizeof(float))); // NOTE** Offset set to skip older indices
    //             // ###
    //             // ### CHECK GENERATION POINTS EXPLICITLY
    //             // ### ENSURE IS ALL CORRECT
    //             // ### DO TEST FOR 2x3 OR SMTHING SMALLER
    //             // ###
    //         }
    //     }
    // }
    glUniformMatrix4fv( glGetUniformLocation(shaderProgram_terrain, "coefficients"), 1, GL_FALSE, glm::value_ptr(terrain_coefficients) );
    for(uint k=0; k<*terrain_z_cells; k++) {
        for(uint i=0; i<*terrain_x_cells; i++) {
            glm::mat4 model = glm::mat4(1.0f);

            // Set uniforms for each entityNEED TO BE UPLOADED EACH TIME? -> CAN YOU DO IT ONCE AND IT CARRY OVER
            glUniformMatrix4fv( glGetUniformLocation(shaderProgram_terrain, "model"), 1, GL_FALSE, glm::value_ptr(model) );

            // Draws the individual pings -> Referring to index offset
            int index_offset = 6*(i +k*(*terrain_x_cells));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(index_offset*sizeof(float))); // NOTE** Offset set to skip older indices
        }
    }

    *prior_t = glfwGetTime();
    glfwSwapBuffers(core_window);
}
void read_keypress_core(GLFWwindow *window, float *pings, int *ping_number, Creature *creatures) {
    if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        std::cout << "glfwGetTime()= " << glfwGetTime() << *ping_number << std::endl;
        std::cout << "    PING COUNT = " << *ping_number << std::endl;
        if(*ping_number > 0) {
            std::cout << "        0th = [" << pings[0] <<", "<< pings[1] <<", "<< pings[2] <<", "<< pings[3] <<", "<< pings[4] <<", "<< pings[5] <<"]"<< std::endl;
        }
    }
    if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if(*ping_number == 0) {
            std::cout << "PING" << std::endl;
            add_ping(pings, ping_number, &(creatures[0]), 1.0f, 1.0f, glfwGetTime());
            add_ping(pings, ping_number, &(creatures[1]), 1.0f, 1.0f, glfwGetTime());
            std::cout << *ping_number << std::endl;
        }
    }
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
        float dir_theta = -3.14f*0.5f;//2.0f*3.14f*rand_float(i*creature_number +4);
        float dir_phi = 3.14f*0.5f;//1.0f*3.14f*rand_float(i*creature_number +5);
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

float fetch_ping_signal_strength(float *pings, int ping_index, float time) {
    /*
    . Returns the signal strength across a ping at the time specified (compares to the original time stored by the ping)
    */
   return exp(-PING_SIGNAL_DECAY_RATE*(time-pings[ping_index*6 +3]));    // NOTE** time-pings[...] = current_t-start_t => should always be positive
}
float fetch_ping_radius(float *pings, int ping_index, float time) {
    /*
    . Returns the radius of the ping currently after it has spread over time and decayed in strength
    */
   return 2.0f*tanh(PING_SIGNAL_DECAY_RATE*(time-pings[ping_index*6 +3]));
}
void add_ping(float *pings, int *ping_number, Entity *entity, float strength, float signal, float time) {
    /*
    . Adds a ping on behalf of another entity
    . Performs checks to ensure the ping is allowed and parameters regarding it are updated

    . strength = strength of the ping signal -> high strength pings will take longer to reach the deacy threshold
    . signal = signal used for this ping -> indicates the 'action' of the ping
    . time = current time (GLFW time)
    */
    if(*ping_number < PING_MAXIMUM) {
        if(time -entity->prev_ping_time >= entity->ping_cooldown) {
            entity->ping(pings, ping_number, time, strength, signal);
            entity->prev_ping_time = time;
            *ping_number = (*ping_number) +1;
        }
    }
}
void remove_pings(float *pings, int *ping_number, float time) {
    /*
    . Removes pings with a signal strength too low (based on decay for current time)

    . time = current time of sim (used to remove pings)
    */
    int ping_rewrite_offset = 0;    // Number of ping spaces back to rewrite a ping (occurs to subsequent pings)
    for(int i=0; i<(*ping_number); i++) {   // *NOTE; 6 elements per ping
        if(fetch_ping_signal_strength(pings, i, time) <= PING_SIGNAL_REMOVE_THRESHOLD) {
            ping_rewrite_offset++;  // => Subsequent pings must be moved
        } else {        // If ping is still going
            if(ping_rewrite_offset > 0) {      // If any prior pings have been removed, rewrite this ping earlier in the sequence
                for(int j=0; j<6; j++) {
                    pings[(i-ping_rewrite_offset)*6 +j] = pings[i*6 +j];
                }
            }
        }
    }
    *ping_number = *ping_number-ping_rewrite_offset;
}
void evaluate_pings(float *pings, int *ping_number, Creature *creatures, const unsigned int creature_number, Resource *resources, const unsigned int resource_number, float time) {
    /*
    . Checks for actions to be completed for each ping
    . E.G.
        Signal | Action
        1       Reflect ping on resource
        2       Make creatures search after hearing ping
        ...
    */
    const int THREAD_NUM = 4;   // Used for parallel search through entities (resources)

    for(int i=0; i<(*ping_number); i++) {
        int found_entity_limit = 10;
        int found_entity_count = 0;
        int found_entity_indices[found_entity_limit] = {};
        float signal = pings[i*6 +5];
        glm::vec3 ping_origin = glm::vec3(pings[i*6 +0], pings[i*6 +1], pings[i*6 +2]);
        float ping_radius = fetch_ping_radius(pings, i, time);
        switch (int(signal)) {  // Consider the signal type (cast to int, but not an issue since always considered int-like anyway)
            case 1:
                // (1) Check if at least 1 resource within radius
                //    (1.5) Send another resource located ping (signal=2) if so --> Only do once every X frames/time passed
                search_entities_spherical(resources, resource_number, ping_origin, ping_radius, found_entity_indices, &found_entity_count, &found_entity_limit, THREAD_NUM);
                for(int i=0; i<found_entity_count; i++) {   // Make a new ping at each found resource
                    add_ping(pings, ping_number, &(resources[found_entity_indices[i]]), 1.0f, 2.0f, time);
                }
                break;
            case 2:
                // (1) Check for all creatures in radius
                //    (1.5) Change all their behaviours to a resource search mode if not already
                //pass
                break;
        }
    }
}
void search_entities_spherical(Entity *entities, const unsigned int entity_number, glm::vec3 origin, float search_radius, int *found_entity_indices, int *found_entity_count, int *found_entity_limit, const int THREAD_NUM) {
    /*
    . Looks for entities in a spherical radius
    . Searches up to max number, then terminates

    . found_entity_limit = Max number of entities to be found before terminating the search
    */
    int thread_entity_count[THREAD_NUM];
    int thread_found_entity_indices[THREAD_NUM*(*found_entity_limit)];  // Each thread has access to found_entity_limit entries, and no more -> collected together at the end
    
    // #pragma omp parallel num_threads(THREAD_NUM) for reduction(+:(*found_entity_count))
    #pragma omp parallel num_threads(THREAD_NUM)
    {
        int thread_index = omp_get_thread_num();
        thread_entity_count[thread_index] = 0;  // Initialise their section as 0
        for(int i=0; i<*found_entity_limit; i++) {thread_found_entity_indices[thread_index*(*found_entity_limit) +i]=-1;}   // Initialise all spaces this thread can use to indices 0
        int start_index = thread_index*int(floor(float(entity_number)/float(THREAD_NUM)));
        int end_index = (thread_index<THREAD_NUM-1) ? (thread_index+1)*int(floor(float(entity_number)/float(THREAD_NUM))) : entity_number;   // NOTE** Ensures last thread covers all remaining entities (after taking floor of divided entity number)

        for(int i=start_index; i<end_index; i++) {
            float separation = sqrt(pow(origin.x-entities[i].position.x,2) +pow(origin.y-entities[i].position.y,2) +pow(origin.z-entities[i].position.z,2));
            if(separation <= search_radius) {
                if(thread_entity_count[thread_index] < *found_entity_limit) {
                    thread_found_entity_indices[ thread_entity_count[thread_index] ] = i;
                    thread_found_entity_indices[ thread_index*THREAD_NUM +thread_entity_count[thread_index] ] = i;
                    thread_entity_count[thread_index] = thread_entity_count[thread_index]+1;
                } else {break;}
            }
        }
    }

    // Gather total count
    for(int i=0; i<THREAD_NUM; i++) {*found_entity_count = *found_entity_count +thread_entity_count[i];}
    // Gather first N elements from all threads (removing the spaces) => could possibly have N*limit values found if all threads max out their limit [ALTERNATIVELY, COULD ONLY TAKE THE FIRST M ELEMENTS FROM THREADS COLLECTIVELY -> HENCE LIMIT OBEYED ENTIRELY]
    int index_gather_count = 0; // Counts correctly re-added indices to the main in set
    for(int i=0; i<THREAD_NUM*(*found_entity_limit); i++) { // Go through everything, re-add anything with no default value (-1)
        if(thread_found_entity_indices[i] != -1) {
            found_entity_indices[index_gather_count] = thread_found_entity_indices[i];
            index_gather_count++;
        }
    }
}

void calculate_terrain_collision(Creature *creatures, const unsigned int creature_number, glm::mat4 terrain_coefficients, float terrain_width) {
    /*
    . Calculates which entities are colliding with the terrain
    . Changes their dyanmics to bounce off the terrain
    */
    // ###
    // ### OMP GOOD HERE
    // ###
    // ###
    // ### MAKE THIS ACTUALLY USE A FRACTION OF THE WIDTH SPECIFIED -> 0.4f
    // ###
    const int THREAD_NUM = 4;
    const float delta_step = terrain_width*0.001f;   // Step used for gradient calculation used in normal calculation

    int elements_per_thread = int(floor(float(creature_number)/float(THREAD_NUM)));

    #pragma omp parallel num_threads(THREAD_NUM)
    {
        int thread_index = omp_get_thread_num();
        int start_index = thread_index*elements_per_thread;
        int end_index = (thread_index<THREAD_NUM-1) ? (thread_index+1)*elements_per_thread : creature_number;
        for(int i=start_index; i<end_index; i++) {
            float terrain_height = terrain_heightmap(creatures[i].position.x, creatures[i].position.z, terrain_coefficients);
            bool creature_colliding = creatures[i].position.y < terrain_height;
            if(creature_colliding) {
                // Move out of terrain
                creatures[i].position.y = terrain_height;
                // Find normal
                float terrain_grad_A = (terrain_heightmap(creatures[i].position.x, creatures[i].position.z, terrain_coefficients) -terrain_heightmap(creatures[i].position.x, creatures[i].position.z +delta_step, terrain_coefficients))/delta_step;  // Calculating a continuous normal for the underlying function, RATHER THAN the grid based approximation visualised
                float terrain_grad_B = (terrain_heightmap(creatures[i].position.x, creatures[i].position.z, terrain_coefficients) -terrain_heightmap(creatures[i].position.x +delta_step, creatures[i].position.z, terrain_coefficients))/delta_step;  // A & B are two perpendicular vectors along the terrain's local surface
                glm::vec3 terrain_normal = glm::normalize(
                    glm::cross(
                        glm::vec3(delta_step, delta_step*terrain_grad_A, 0.0f),
                        glm::vec3(0.0f, delta_step*terrain_grad_B, delta_step)
                    )
                );  // Unit normal vector
                // Reflect velocity
                glm::vec3 velocity_perp = terrain_normal*float(glm::dot(-creatures[i].velocity, terrain_normal) / ( pow(terrain_normal.x, 2) +pow(terrain_normal.y, 2) +pow(terrain_normal.z, 2) ));
                creatures[i].velocity = (creatures[i].velocity +velocity_perp) +velocity_perp*TERRAIN_REFLECT_FACTOR;
            }
        }
    }
}

int fetch_terrain_vertex_data_size(int x_cells, int z_cells) { return (x_cells+1)*(z_cells+1)*2; }  // [X,Z,...] point for each vertex (Y given as a uniform)
int fetch_terrain_vertex_indices_size(int x_cells, int z_cells) { return (x_cells)*(z_cells)*3*2; }  // [I1, I2, I3,...] for each tri (3 indices), 2 tris per square grid, fence post problem for number of squares
void generate_terrain_vertex_info(int x_cells, int z_cells, float cell_width, float *terrain_vertex_data, int *terrain_vertex_indices) {
    /*
    . Generates and populates an empty terrain vertex_data and vertex_indices array with relevent information
    . This is performed as a 1-time call when the program begins -> the grid does not change after this point

    . x/z_cells = number of cells in the X and Z axes of the grid (its width and depth)
    . cell_width = float width/depth (square) dimensions for each cell -> resolution for how often the height map will be sampled
    */
    // Vertex data
    for(uint k=0; k<z_cells+1; k++) {
        for(uint i=0; i<x_cells+1; i++) {
            terrain_vertex_data[2*(i +k*(x_cells+1))   ] = i*cell_width -(x_cells*cell_width/2.0f);   // X coord
            terrain_vertex_data[2*(i +k*(x_cells+1)) +1] = k*cell_width -(z_cells*cell_width/2.0f);   // Z coord
        }
    }

    // Vertex indices
    for(uint k=0; k<z_cells; k++) {
        for(uint i=0; i<x_cells; i++) {
            // For each cell, add 6 elements (2 triangles) in order
            // NOTE** x_cells+1 used since X posts=>X+1 vertices per layer
            terrain_vertex_indices[6*(i +k*x_cells) +0] = i +k*(x_cells+1);                 //  0|*
            terrain_vertex_indices[6*(i +k*x_cells) +1] = i +k*(x_cells+1) +(x_cells+1);    //   | *
            terrain_vertex_indices[6*(i +k*x_cells) +2] = i +k*(x_cells+1) +(x_cells+1) +1; //  1|__*_2

            terrain_vertex_indices[6*(i +k*x_cells) +3] = i +k*(x_cells+1) +(x_cells+1) +1; //  4-*--|5
            terrain_vertex_indices[6*(i +k*x_cells) +4] = i +k*(x_cells+1);                 //     * |
            terrain_vertex_indices[6*(i +k*x_cells) +5] = i +k*(x_cells+1) +1;              //      *|3
        }
    }
}
void generate_terrain_coefficients(int matrix_width, glm::mat4 *coefficient_matrix, int seed) {
    /*
    . Coefficients for a terrain height map
    . Set once at the start (NOT re-generated)
    */
    // ###
    // ### Only works for 4x4 matricies for now -> Not a pressing issue to generalise this
    // ###
    for(int j=0; j<matrix_width; j++) {
        for(int i=0; i<matrix_width; i++) {
            // (*coefficient_matrix)[j][i] = rand_float(seed +i +j*matrix_width);  // Each in range (0,1)
            (*coefficient_matrix)[j][i] = 0.2f;  // Each in range (0,1)
        }
    }
}

float terrain_heightmap(float x, float z, glm::mat4 terrain_coefficients) {
    /*
    . Height map used to generate/render terrain
    . Is continuous, but sampled on a grid

    NOTE; This is must MANUALLY be set to calculate the height map the SAME as the shader being used for the terrain, otherwise 
        visuals and collision will disagree.
    */
    float height = 0.0f;
    for(int i=0; i<16; i++) {   // 16 for 4x4 matrix elements
        if(i%2==0) {
            height += terrain_coefficients[int(floor(float(i)/4.0f))][i%4]*sin( i*3.14f*x*0.04f )*0.35f;
        } else {
            height += terrain_coefficients[int(floor(float(i)/4.0f))][i%4]*sin( i*3.14f*z*0.04f )*0.35f;
        }
    }
    return height;
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
