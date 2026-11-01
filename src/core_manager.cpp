#include <iostream>
#include "glad.h"

#include "core_manager.h"
#include "custom_random.h"

Core_Manager::Core_Manager(unsigned int screen_width, unsigned int screen_height) {
    this->screen_width = screen_width;
    this->screen_height = screen_height;
}
Core_Manager::~Core_Manager() {
    // Clean-up buffers established
    glDeleteVertexArrays(1, &VAO_entity);
    glDeleteVertexArrays(1, &VBO_entity);
    glDeleteVertexArrays(1, &EBO_entity);

    glDeleteVertexArrays(1, &VAO_ping);
    glDeleteVertexArrays(1, &VBO_ping);
    glDeleteVertexArrays(1, &EBO_ping);
     
    glDeleteVertexArrays(1, &VAO_entity);
    glDeleteVertexArrays(1, &VBO_entity);
    glDeleteVertexArrays(1, &EBO_entity);
}

void Core_Manager::setup() {
    // Fill terrain grid structure + coefficients for height map
    generate_terrain_vertex_info(x_cells, z_cells, cell_width, terrain_vertex_data, terrain_vertex_indices);
    generate_terrain_coefficients(&terrain_coefficients, 0);
    
    // Add creatures + resources to lists
    // populate_creatures(spawn_dimensions);
    populate_creatures_testing(0, spawn_dimensions);
    // populate_resources(spawn_dimensions);

    // Setup rendering objects --> Build buffers AFTER vertex data is generated
    setup_shaders();
    setup_buffers();
    setup_models();
}
void Core_Manager::setup_shaders() {
    // NOTE** Make sure you have window focus before this setup

    // Setup shaders + OpenGL arrays
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int vertexShaderID;    // Temporary variables -> overwritten and deleted after each shader setup
    unsigned int fragmentShaderID;  //

    // ENTITY SHADER
    shaderProgram_entities = glCreateProgram();

    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &vertexShader_entities, NULL);
    glCompileShader(vertexShaderID);
    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragmentShader_entities, NULL);
    glCompileShader(fragmentShaderID);

    glAttachShader(shaderProgram_entities, vertexShaderID);
    glAttachShader(shaderProgram_entities, fragmentShaderID);
    glLinkProgram(shaderProgram_entities);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    // PING SPHERE SHADER
    shaderProgram_pingSphere = glCreateProgram();

    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &vertexShader_pingSphere, NULL);
    glCompileShader(vertexShaderID);
    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragmentShader_pingSphere, NULL);
    glCompileShader(fragmentShaderID);

    glAttachShader(shaderProgram_pingSphere, vertexShaderID);
    glAttachShader(shaderProgram_pingSphere, fragmentShaderID);
    glLinkProgram(shaderProgram_pingSphere);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    // TERRAIN SHADER
    shaderProgram_terrain = glCreateProgram();

    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &vertexShader_terrain, NULL);
    glCompileShader(vertexShaderID);
    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragmentShader_terrain, NULL);
    glCompileShader(fragmentShaderID);

    glAttachShader(shaderProgram_terrain, vertexShaderID);
    glAttachShader(shaderProgram_terrain, fragmentShaderID);
    glLinkProgram(shaderProgram_terrain);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    // MODEL_LOADING SHADER
    shaderProgram_modelLoading = glCreateProgram();

    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &vertexShader_modelLoading, NULL);
    glCompileShader(vertexShaderID);
    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragmentShader_modelLoading, NULL);
    glCompileShader(fragmentShaderID);

    glAttachShader(shaderProgram_modelLoading, vertexShaderID);
    glAttachShader(shaderProgram_modelLoading, fragmentShaderID);
    glLinkProgram(shaderProgram_modelLoading);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
}
void Core_Manager::setup_buffers() {
    // Make + Setup all VBO, VAO, EBO buffers
    // ---- STANDARD MESHES ----
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

    glGenBuffers(1, &VBO_ping);   // Generate 1 buffer, give ID to VAO_entity
    glBindBuffer(GL_ARRAY_BUFFER, VBO_ping);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ping_sphere_vertex_data), ping_sphere_vertex_data, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &VAO_ping);   // Generate 1 buffer, give ID to VAO_entity
    glBindVertexArray(VAO_ping);

    glGenBuffers(1, &EBO_ping);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_ping);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ping_sphere_vertex_indices), ping_sphere_vertex_indices, GL_STATIC_DRAW);    // This object ONLY interested in indices -> Vertex data pulled from bound VBO

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // ---- TERRAIN MESHES ----
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
}
void Core_Manager::setup_models() {
    // Flips texture Y according to OpenGL opposite sense
    stbi_set_flip_vertically_on_load(true);
    // ...
}

void Core_Manager::read_keypresses(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        std::cout << "glfwGetTime()= " << glfwGetTime() << std::endl;
        std::cout << "    PING COUNT = " << pings.size() << std::endl;
    }
    if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if(pings.size() == 0) {
            std::cout << "PING" << std::endl;
            ping_entity(creatures[0], 1.0f, 1, glfwGetTime());
            ping_entity(creatures[1], 1.0f, 1, glfwGetTime());
            std::cout << pings.size() << std::endl;
        }
    }

    // Test Pings
    if(glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        if(pings.size() == 0) {
            std::cout << "PING" << std::endl;
            add_ping(glm::vec3(0.0f,  0.0f, 0.0f), 1.0f, 1, glfwGetTime());
            add_ping(glm::vec3(0.8f*PING_MAX_RADIUS, 10.0f, 0.0f), 1.0f, 1, glfwGetTime());
            std::cout << pings.size() << std::endl;
        }
    }
}

void Core_Manager::render(GLFWwindow *window) {
    glfwMakeContextCurrent(window); // Ensure this window is the active window

    calculate_entities();
    calculate_camera_controls(window, &(this->camera_origin), &(this->camera_theta), &(this->camera_phi), &(this->camera_fov));
    read_keypresses(window);

    delta_t = (glfwGetTime()-prior_t +0.0000001f);    // **NOTE; +0.0000001f included to prevent /0 errors
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, this->camera_origin);
    view = glm::rotate(view, this->camera_theta, glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::rotate(view, this->camera_phi, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 projection = glm::mat4(1.0f);     // "" ""
    projection = glm::perspective(glm::radians(this->camera_fov), (float)screen_width / (float)screen_height, 0.1f, 100.0f);

    render_entities(window, view, projection);
    render_pings(window, view, projection);
    render_terrain(window, view, projection);
    render_outpost(window, view, projection);

    prior_t = glfwGetTime();
    glfwSwapBuffers(window);
}
void Core_Manager::render_entities(GLFWwindow *window, glm::mat4 view, glm::mat4 projection) {
    glUseProgram(shaderProgram_entities);    // Uses the shader compiled

    glUniformMatrix4fv( glGetUniformLocation(shaderProgram_entities, "view"), 1, GL_FALSE, glm::value_ptr(view) );
    glUniformMatrix4fv( glGetUniformLocation(shaderProgram_entities, "projection"), 1, GL_FALSE, glm::value_ptr(projection) );

    glBindVertexArray(VAO_entity);   // Updates/resets VAO_entity for next frame

    // Draw creatures
    // std::cout << "--CREATURES--" << std::endl;
    for(uint i=0; i<creatures.size(); i++) {
        // Draw each entity
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, creatures[i]->position);

        // Set uniforms for each entity
        glUniformMatrix4fv( glGetUniformLocation(shaderProgram_entities, "model"), 1, GL_FALSE, glm::value_ptr(model) );

        // Draws the individual entities -> First 36, 0 offset
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    // Draw resources
    // std::cout << "--RESOURCES--" << std::endl;
    for(uint i=0; i<resources.size(); i++) {
        // std::cout << "  position=" << resources[i].position.x << ", " << resources[i].position.y << ", " << resources[i].position.z << "]" << std::endl;
        // Draw each resource
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, resources[i]->position);

        // Set uniforms for each entity
        glUniformMatrix4fv( glGetUniformLocation(shaderProgram_entities, "model"), 1, GL_FALSE, glm::value_ptr(model) );

        // Draws the individual entities -> Next 24 tris, 36 offset
        glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, (void*)(36*sizeof(float)));
    }
}
void Core_Manager::render_pings(GLFWwindow *window, glm::mat4 view, glm::mat4 projection) {
    glUseProgram(shaderProgram_pingSphere);    // Uses the shader compiled
    glUniformMatrix4fv( glGetUniformLocation(shaderProgram_pingSphere, "view"), 1, GL_FALSE, glm::value_ptr(view) );
    glUniformMatrix4fv( glGetUniformLocation(shaderProgram_pingSphere, "projection"), 1, GL_FALSE, glm::value_ptr(projection) );
    glBindVertexArray(VAO_ping);   // Swaps to the other VAO/EBO needed

    for(uint i=0; i<(pings.size()); i++) {
        // Centre each ping
        float radius = fetch_ping_radius(pings[i], glfwGetTime());
        float strength = fetch_ping_signal_strength(pings[i], glfwGetTime());
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pings[i]->position);
        model = glm::scale(model, glm::vec3(radius));    // NOTE** Model vertex coords has a default radius of 1f, hence can just multiply by radius here
        glm::vec4 colourData = glm::vec4(1.0f, 1.0f, 1.0f, strength-PING_SIGNAL_REMOVE_THRESHOLD);
        if     (pings[i]->signal == 1) { colourData = glm::vec4(1.0f, 0.0f, 0.0f, strength-PING_SIGNAL_REMOVE_THRESHOLD); }  // Search for resources 
        else if(pings[i]->signal == 2) { colourData = glm::vec4(0.0f, 1.0f, 0.0f, strength-PING_SIGNAL_REMOVE_THRESHOLD); }  // Resource return
        else if(pings[i]->signal == 3) { colourData = glm::vec4(0.0f, 0.0f, 1.0f, strength-PING_SIGNAL_REMOVE_THRESHOLD); }  // Search for catchers
        
        // Set uniforms for each entity
        glUniformMatrix4fv( glGetUniformLocation(shaderProgram_pingSphere, "model"), 1, GL_FALSE, glm::value_ptr(model) );
        glUniform4fv( glGetUniformLocation(shaderProgram_pingSphere, "colourData"), 1, glm::value_ptr(colourData) );

        // Draws the individual pings -> Referring to index offset
        glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, (void*)(0)); // NOTE** Offset set to skip older indices
    }
}
void Core_Manager::render_terrain(GLFWwindow *window, glm::mat4 view, glm::mat4 projection) {
    // Set shader + buffers + uniforms
    glUseProgram(shaderProgram_terrain);    // Uses the shader compiled
    // glBindVertexArray(VAO_terrain);   // Swaps to the other VAO/EBO needed

    glUniformMatrix4fv( glGetUniformLocation(shaderProgram_terrain, "view"), 1, GL_FALSE, glm::value_ptr(view) );
    glUniformMatrix4fv( glGetUniformLocation(shaderProgram_terrain, "projection"), 1, GL_FALSE, glm::value_ptr(projection) );
    glBindVertexArray(VAO_terrain);   // Swaps to the other VAO/EBO needed
    glUniformMatrix4fv( glGetUniformLocation(shaderProgram_terrain, "coefficients"), 1, GL_FALSE, glm::value_ptr(terrain_coefficients) );

    for(uint k=0; k<z_cells; k++) {
        for(uint i=0; i<x_cells; i++) {
            glm::mat4 model = glm::mat4(1.0f);

            // Set uniforms for each entityNEED TO BE UPLOADED EACH TIME? -> CAN YOU DO IT ONCE AND IT CARRY OVER
            glUniformMatrix4fv( glGetUniformLocation(shaderProgram_terrain, "model"), 1, GL_FALSE, glm::value_ptr(model) );

            // Draws the individual pings -> Referring to index offset
            // int index_offset = 6*(i +k*(this->x_cells));
            int index_offset = 6*(i +k*x_cells);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(index_offset*sizeof(float))); // NOTE** Offset set to skip older indices
        }
    }
}

void Core_Manager::render_outpost(GLFWwindow *window, glm::mat4 view, glm::mat4 projection) {
    // Set shader + buffers + uniforms
    glUseProgram(shaderProgram_modelLoading);    // Uses the shader compiled

    glUniformMatrix4fv( glGetUniformLocation(shaderProgram_modelLoading, "view"), 1, GL_FALSE, glm::value_ptr(view) );
    glUniformMatrix4fv( glGetUniformLocation(shaderProgram_modelLoading, "projection"), 1, GL_FALSE, glm::value_ptr(projection) );

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, outpost_origin);
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    glUniformMatrix4fv( glGetUniformLocation(shaderProgram_modelLoading, "model"), 1, GL_FALSE, glm::value_ptr(model) );
    
    outpost_model.render(shaderProgram_modelLoading);
}


void Core_Manager::populate_creatures(float *spawn_dimemsions) {
    /*
    . Goes through each empty entity in a list and populates it with randomised starting data

    . creatures = list of creatures
    . spawn_dimenions = [X,Y,Z] widths for spawnable half-width (+- allowed)
    */
    // for(int i=0; i<(creatures.size()); i++) {
    //     Creature newCreature = Creature();

    //     newCreature.creature_type = 1;

    //     newCreature.id = i;
    //     newCreature.position = glm::vec3(
    //         spawn_dimemsions[0]*(0.5f -Custom_Random::rFloat())*2,
    //         spawn_dimemsions[1]*(0.5f -Custom_Random::rFloat())*2,
    //         spawn_dimemsions[2]*(0.5f -Custom_Random::rFloat())*2
    //     );
    //     float dir_theta = -3.14f*0.5f;//2.0f*3.14f*Custom_Random::rFloat();
    //     float dir_phi = 3.14f*0.5f;//1.0f*3.14f*Custom_Random::rFloat();
    //     newCreature.forward_direction = glm::vec3(cos(dir_theta)*sin(dir_phi), sin(dir_theta)*sin(dir_phi), cos(dir_phi));
    //     newCreature.up_direction = glm::vec3(cos(dir_theta)*sin(dir_phi-3.14f/2.0f), sin(dir_theta)*sin(dir_phi-3.14f/2.0f), cos(dir_phi-3.14f/2.0f));
    //     creatures[i] = newCreature;
    // }
}
void Core_Manager::populate_creatures_testing(int scenario, float *spawn_dimemsions) {
    /*
    . Used for testing ping behaviours
    . Generates creatures according to a scenario index
    . Tells the user if parameters are not correct for a given scenario

    . creatures = list of creatures
    . spawn_dimenions = [X,Y,Z] widths for spawnable half-width (+- allowed)
    */
    if(scenario==0) {
        Creature *newCreature_A = new Creature();
        newCreature_A->creature_type = 0;
        newCreature_A->id = 0;  // Being checked by bar figure
        newCreature_A->position = glm::vec3(
            0.0f,
            10.0f,
            0.0f
        );
        creatures.push_back(newCreature_A);

        Creature *newCreature_B = new Creature();
        newCreature_B->creature_type = 0;
        newCreature_B->id = 0;
        newCreature_B->position = glm::vec3(
            0.8f*PING_MAX_RADIUS,
            10.0f,
            0.0f
        );
        creatures.push_back(newCreature_B);


        Resource *newResource_A = new Resource();
        newResource_A->resource_type = 1;
        newResource_A->id = 0;
        newResource_A->position = glm::vec3(
            0.0f,
            15.0f,
            0.0f
        );
        resources.push_back(newResource_A);

        Resource *newResource_B = new Resource();
        newResource_B->resource_type = 1;
        newResource_B->id = 0;
        newResource_B->position = glm::vec3(
            1.6f*PING_MAX_RADIUS,
            10.0f,
            0.0f
        );
        resources.push_back(newResource_B);
    }
}
void Core_Manager::populate_resources(float *spawn_dimemsions) {
    /*
    . Goes through each empty entity in a list and populates it with randomised starting data

    . resources = list of resources
    . spawn_dimenions = [X,Y,Z] widths for spawnable half-width (+- allowed)
    */
    // for(int i=0; i<(resources.size()); i++) {
    //     Resource newResource = Resource();

    //     newResource.resource_type = 1;

    //     newResource.id = i;
    //     newResource.position = glm::vec3(
    //         spawn_dimemsions[0]*(0.5f -Custom_Random::rFloat())*2,
    //         spawn_dimemsions[1]*(0.5f -Custom_Random::rFloat())*2,
    //         spawn_dimemsions[2]*(0.5f -Custom_Random::rFloat())*2
    //     );
    //     float dir_theta = 2.0f*3.14f*Custom_Random::rFloat();
    //     float dir_phi = 1.0f*3.14f*Custom_Random::rFloat();
    //     newResource.forward_direction = glm::vec3(cos(dir_theta)*sin(dir_phi), sin(dir_theta)*sin(dir_phi), cos(dir_phi));
    //     newResource.up_direction = glm::vec3(cos(dir_theta)*sin(dir_phi-3.14f/2.0f), sin(dir_theta)*sin(dir_phi-3.14f/2.0f), cos(dir_phi-3.14f/2.0f));
    //     resources[i] = newResource;
    // }
}

int Core_Manager::fetch_terrain_vertex_data_size(int x_cells, int z_cells) { return (x_cells+1)*(z_cells+1)*2; }  // [X,Z,...] point for each vertex (Y given as a uniform)
int Core_Manager::fetch_terrain_vertex_indices_size(int x_cells, int z_cells) { return (x_cells)*(z_cells)*3*2; }  // [I1, I2, I3,...] for each tri (3 indices), 2 tris per square grid, fence post problem for number of squares
void Core_Manager::generate_terrain_vertex_info(int x_cells, int z_cells, float cell_width, float *terrain_vertex_data, int *terrain_vertex_indices) {
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
void Core_Manager::generate_terrain_coefficients(glm::mat4 *coefficient_matrix, int seed) {
    /*
    . Coefficients for a terrain height map
    . Set once at the start (NOT re-generated)
    . ALWAYS a 4x4 matrix of coeffs currently
    */
    for(int j=0; j<4; j++) {
        for(int i=0; i<4; i++) {
            (*coefficient_matrix)[j][i] = Custom_Random::rFloat();  // Each in range (0,1)
            // (*coefficient_matrix)[j][i] = 0.2f;  // Each in range (0,1)
        }
    }
}

float Core_Manager::terrain_heightmap(float x, float z, glm::mat4 terrain_coefficients) {
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

void Core_Manager::calculate_entities() {
    /*
    . Performs calculations for entities
    . These include sending and processing pings, updating dynamics, etc
    */
    float time = glfwGetTime();
    update_creatures(time);
    evaluate_pings(time);
    calculate_terrain_collision(creatures, terrain_coefficients, cell_width);
    remove_pings(&(pings), time);
}
float Core_Manager::fetch_ping_signal_strength(Ping *ping, float time) {
    /*
    . Returns the signal strength across a ping at the time specified (compares to the original time stored by the ping)
    */
   return ping->strength*exp(-PING_SIGNAL_DECAY_RATE*(time-ping->time));    // NOTE** time-pings[...] = current_t-start_t => should always be positive
}
float Core_Manager::fetch_ping_radius(Ping *ping, float time) {
    /*
    . Returns the radius of the ping currently after it has spread over time and decayed in strength
    */
   return PING_MAX_RADIUS*tanh(PING_SIGNAL_DECAY_RATE*(time-ping->time));
}
void Core_Manager::ping_entity(Entity *entity, float strength, int signal, float time) {
    /*
    . Attempts to ping at an entity
    . This is subject to entity ping cooldowns
    */#
    if(time -entity->prev_ping_time >= entity->ping_cooldown) { // If ping is off-cooldown, add the ping
        bool ping_successful = add_ping(entity->position, strength, signal, time);
        if(ping_successful) {
            entity->prev_ping_time = time;
        }    // Only set the cooldown if the ping actually occurred
    }
}
bool Core_Manager::add_ping(glm::vec3 position, float strength, int signal, float time) {
    /*
    . Adds a ping on behalf of another entcreature_numberity
    . Performs checks to ensure the ping is allowed and parameters regarding it are updated
    . Returns whether the ping was added or not

    . strength = strength of the ping signal -> high strength pings will take longer to reach the deacy threshold
    . signal = signal used for this ping -> indicates the 'action' of the ping
    . time = current time (GLFW time)
    */
    // ###
    // ### PING_MAXIMUM A LEGACY IDEA NOW SO CAN REMOVE THIS -> IS A NICE SAFETY NET HOWEVER
    // ###
    if(pings.size() < PING_MAXIMUM) {   // If you have space for more pings
        // Add ping data
        Ping *newPing = new Ping;
        newPing->position = position;
        newPing->time     = time;
        newPing->strength = strength;
        newPing->signal   = signal;
        pings.push_back(newPing);
        return true;
    } else {return false;}
}
void Core_Manager::remove_pings(std::vector<Ping*> *pings, float time) {
    /*
    . Removes pings with a signal strength too low (based on decay for current time)

    . time = current time of sim (used to remove pings)
    */
    const int original_length = pings->size();
    for(int i=original_length-1; i>=0; i--) {
        if(fetch_ping_signal_strength((*pings)[i], time) <= PING_SIGNAL_REMOVE_THRESHOLD) { // If signal gets too low, remove the ping
            delete (*pings)[i];                 // Free up memory
            pings->erase(pings->begin()+i);     // Remove from list
        }   // If signal strength is large enough, leave it be
    }
}
void Core_Manager::evaluate_pings(float time) {
    /*
    . Checks for actions to be completed for each ping
    . E.G.
        Signal | Action
        1       Reflect ping on resource
        2       Make creatures search after hearing ping
        ...
    */
    const int THREAD_NUM = 4;   // Used for parallel search through entities (resources)

    for(int i=0; i<(pings.size()); i++) {
        std::vector<int> found_entity_indices;
        float ping_radius = fetch_ping_radius(pings[i], time);
        switch(pings[i]->signal) {  // Consider the signal type (cast to int, but not an issue since always considered int-like anyway)
            case 1:
                /*
                . Detects nearby resources -> Resources emit a ping (signal==2.0) if they encounter this signal

                . (1) Check if at least 1 resource within radius
                .   (2) Send another resource located ping (signal=2) if so --> Only do once every X frames/time passed
                */
                search_entities_spherical<Resource*>(resources, pings[i]->position, ping_radius, &found_entity_indices, THREAD_NUM);
                for(int j=0; j<found_entity_indices.size(); j++) {   // Make a new ping at each found resource
                    ping_entity(resources[found_entity_indices[j]], 1.0f, 2, time);
                }
                break;
            case 2:
                /*
                . If creature detects this signal, try to travel towards it

                . (1) Check for all creatures in radius -> choose the closest
                .   (2) Closest atrget has this object stored in its memory (will naturally try to move towards a target if it has one, forgets after a cooldown)
                */
                search_entities_spherical<Creature*>(creatures, pings[i]->position, ping_radius, &found_entity_indices, THREAD_NUM);
                for(int j=0; j<found_entity_indices.size(); j++) {   // If a creature hears this, change their behaviour (movement change + emit smaller pings to detect resource nearby)
                    // NOTE** Currently all creatures who hear this will travel to the resource -> this is fine but means need to prepare for another entity to try and 'steal' the resource by moving it before another
                    if(creatures[found_entity_indices[j]]->prior_ping_signal != 2) {
                        creatures[found_entity_indices[j]]->behaviour = 1;
                        creatures[found_entity_indices[j]]->prior_ping_signal = 2;
                        creatures[found_entity_indices[j]]->prior_ping_direction = glm::normalize(pings[i]->position -creatures[found_entity_indices[j]]->position);
                    } else {    // If previous signal valus is 2, then look for how close you are instead to try and switch behaviours again (throwing mode)
                        // ###
                        // ### MOVE DIST CALC OUT FOR LESS WASTED CALULATIONS
                        // ###
                        if(glm::distance(pings[i]->position, creatures[found_entity_indices[j]]->position) < 0.15f*PING_MAX_RADIUS) {
                            creatures[found_entity_indices[j]]->memory_vector3 = glm::vec3(creatures[found_entity_indices[j]]->position.x +0.0001f, creatures[found_entity_indices[j]]->position.y +0.0001f, creatures[found_entity_indices[j]]->position.z +0.0001f);
                            creatures[found_entity_indices[j]]->behaviour = 2;  // If close enough; switch to thrower behaviour
                        } else if(glm::distance(pings[i]->position, creatures[found_entity_indices[j]]->position)  >= 4.0f*PING_MAX_RADIUS) {
                            creatures[found_entity_indices[j]]->memory_vector3 = glm::vec3(0.0f);
                            creatures[found_entity_indices[j]]->behaviour = 0;  // If too far; revert back to wandering behaviour
                        }
                    }
                }
                break;
            case 3:
                /*
                . Sent out when a creature is sufficiently close to a resource (on top of it)
                . Other creatures who hear this will try enter a state to try catch the resource (target is the closest creature in range who is nearest to the outpost + must be closer than this creature)
                . Once they hear this, they will send a signal to let the thrower know (1) that they are ready to catch +(2) where they are [so the thrower can check if they are closer than them to the outpost]
                */
               break;
        }
    }
}
void Core_Manager::update_creatures(float time) {
    /*
    . Updates various behaviours of creatures such as;
        - Dynamics
        - Behaviour
        ...
    */
    std::cout << "---" << std::endl;
    for(int i=0; i<creatures.size(); i++) {
        // ###
        // ### CHECK BEHAVIOURS
        // ### MAKE NOT SPAM SIGNAL 1
        // ### MAKE SURE IS DOING SIGNAL 3 AND ACTUALLY WAITING
        // ###
        std::cout << "Behaviour, " << i << " = " << creatures[i]->behaviour << std::endl;

        // Update physics for each entity
        switch(creatures[i]->behaviour) {
            case 0:
                // Idle/Wander
                // creatures[i]->directional_thrust(-creatures[i]->position, creatures[i]->travel_speed);
                if(int(floor(time)) % 2 == 0) { creatures[i]->ping_toggle = true; } // Periodically set to true (ready for other behaviours)
                break;
            case 1:
                // Travel to last seen resource (if any)
                if(creatures[i]->prior_ping_signal == 2) {  // If your behaviour is to chase a resource, and the last ping you heard was from a resource, travel to its location
                    creatures[i]->directional_thrust(creatures[i]->prior_ping_direction, creatures[i]->travel_speed);
                }
                // Do short but frequent pings to see if a resource is near
                if( (creatures[i]->ping_toggle) && (int(floor(time)) % 2 == 0) ) {     // Every X seconds, emit this tiny ping
                    // ping_entity(creatures[i], 0.5f, 1, time);
                    add_ping(creatures[i]->position, 0.5f, 1, time);    // NOTE** Only doing it this way to bypass the cooldown -> Should just adjust how the cooldowns work (1 for emitting 1 for hearing)
                    creatures[i]->ping_toggle = false;
                } if(int(floor(time)) % 2 != 0) {creatures[i]->ping_toggle = true;} // Reset on odds, triggers on evens
                // Change behaviour again if resource is close enough
                break;
            case 2:
                // Try to stop at the current location
                creatures[i]->directional_thrust(
                    glm::normalize(creatures[i]->memory_vector3 -creatures[i]->position), 
                    3.0f*creatures[i]->travel_speed  //*glm::distance(creatures[i]->memory_vector3, creatures[i]->position)
                );
                std::cout << "memory_vector3 = [" <<creatures[i]->memory_vector3.x<<", "<<creatures[i]->memory_vector3.y<<", "<<creatures[i]->memory_vector3.z<<", " << std::endl;

                // Ping to see if another entity is nearby to catch a thrown resource
                if( (creatures[i]->ping_toggle) && (int(floor(time)) % 5 == 0) ) {
                    ping_entity(creatures[i], 1.5f, 3, time);   // Signal 3 is call for catchers
                } if(int(floor(time)) % 5 != 0) {creatures[i]->ping_toggle = true;}
                break;
            //...
        }
        // creatures[i].directional_thrust(creatures[i].forward_direction, 0.005f);
        // float dist = std::max<float>( sqrt( pow(creatures[i]->position.x, 2) + pow(creatures[i]->position.y, 2) + pow(creatures[i]->position.z, 2) ), 0.0000001f);

        creatures[i]->update_dynamics(delta_t);
    }
}

void Core_Manager::calculate_terrain_collision(std::vector<Creature*> creatures, glm::mat4 terrain_coefficients, float terrain_width) {
    /*
    . Calculates which entities are colliding with the terrain
    . Changes their dyanmics to bounce off the terrain
    */
    const int THREAD_NUM = 4;
    const float delta_step = terrain_width*0.001f;   // Step used for gradient calculation used in normal calculation

    int elements_per_thread = int(floor(float(creatures.size())/float(THREAD_NUM)));

    #pragma omp parallel num_threads(THREAD_NUM)
    {
        int thread_index = omp_get_thread_num();
        int start_index = thread_index*elements_per_thread;
        int end_index = (thread_index<THREAD_NUM-1) ? (thread_index+1)*elements_per_thread : creatures.size();
        for(int i=start_index; i<end_index; i++) {
            float terrain_height = terrain_heightmap(creatures[i]->position.x, creatures[i]->position.z, terrain_coefficients);
            bool creature_colliding = creatures[i]->position.y < terrain_height;
            if(creature_colliding) {
                // Move out of terrain
                creatures[i]->position.y = terrain_height;
                // Find normal
                float terrain_grad_A = (terrain_heightmap(creatures[i]->position.x, creatures[i]->position.z, terrain_coefficients) -terrain_heightmap(creatures[i]->position.x, creatures[i]->position.z +delta_step, terrain_coefficients))/delta_step;  // Calculating a continuous normal for the underlying function, RATHER THAN the grid based approximation visualised
                float terrain_grad_B = (terrain_heightmap(creatures[i]->position.x, creatures[i]->position.z, terrain_coefficients) -terrain_heightmap(creatures[i]->position.x +delta_step, creatures[i]->position.z, terrain_coefficients))/delta_step;  // A & B are two perpendicular vectors along the terrain's local surface
                glm::vec3 terrain_normal = glm::normalize(
                    glm::cross(
                        glm::vec3(delta_step, delta_step*terrain_grad_A, 0.0f),
                        glm::vec3(0.0f, delta_step*terrain_grad_B, delta_step)
                    )
                );  // Unit normal vector
                // Reflect velocity
                glm::vec3 velocity_perp = terrain_normal*float(glm::dot(-creatures[i]->velocity, terrain_normal) / ( pow(terrain_normal.x, 2) +pow(terrain_normal.y, 2) +pow(terrain_normal.z, 2) ));
                creatures[i]->velocity = (creatures[i]->velocity +velocity_perp) +velocity_perp*TERRAIN_REFLECT_FACTOR;
            }
        }
    }
}
