#ifndef CORE_MANAGER_H
#define CORE_MANAGER_H

#include <filesystem>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "camera_controls.h"

#include "simulation_consts.h"
#include "creature.h"
#include "resource.h"
#include "model.h"

class Core_Manager {
    public:
        const char *vertexShader_entities = "#version 330 core\n"
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

        const char *fragmentShader_entities = "#version 330 core\n"
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


        const char *vertexShader_modelLoading = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "layout (location = 1) in vec3 aNormal;\n"
            "layout (location = 2) in vec2 aTexCoords;\n"
            "out vec2 TexCoords;\n"
            "uniform mat4 model;\n"
            "uniform mat4 view;\n"
            "uniform mat4 projection;\n"
            "void main()\n"
            "{\n"
            "    TexCoords = aTexCoords;\n"
            "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
            "}\n";
        const char *fragmentShader_modelLoading = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "in vec2 TexCoords;\n"
            "uniform sampler2D texture_diffuse1;\n"
            "void main()\n"
            "{\n"
            "    FragColor = texture(texture_diffuse1, TexCoords);\n"
            "}\n";

        unsigned int screen_width;
        unsigned int screen_height;

        static const unsigned int creature_number = 100;
        static const unsigned int resource_number = 10;
        Creature creatures[creature_number];
        Resource resources[resource_number];
        // ###
        // ### SWITCH BOTH OF THESE TO VECTOR LISTS FOR VARIABLE ENTITY COUNTS ###
        // ###

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


        unsigned int shaderProgram_entities;
        unsigned int shaderProgram_pingSphere;
        unsigned int shaderProgram_terrain;
        unsigned int shaderProgram_modelLoading;


        float entity_vertex_data[14*6] = {
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
        }; // Creature + resource /w pos + colour data
        int entity_vertex_indices[20*3] = {
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

        float ping_sphere_vertex_data[6*3] = {
            1.0f, 0.0f, 0.0f,      // 4 Planar vertices
            0.0f, 0.0f, -1.0f,
            -1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f,      // Top
            0.0f, -1.0f, 0.0f,     // Bottom
        };      // [X,Y,Z, R,G,B]
        int ping_sphere_vertex_indices[8*3] = {
            0,4,1,     // Upper faces
            1,4,2,
            2,4,3,
            3,4,0,
            0,5,1,    // Lower faces
            1,5,2,
            2,5,3,
            3,5,0
        };      // [I1, I2, I3]


        static const int x_cells = 50;
        static const int z_cells = 100;
        static constexpr float cell_width = 0.4f;
        float terrain_vertex_data[ (x_cells+1)*(z_cells+1)*2 ];     // Initialise blank for generation next
        int terrain_vertex_indices[ (x_cells)*(z_cells)*3*2 ];    //
        glm::mat4 terrain_coefficients = glm::mat4(0.0f);

        // Outpost data
        const std::string model_path_dir = "/home/james-paget/Desktop/OpenGL_OMP_Example/models/outpost_model.obj";
        Model outpost_model = Model(model_path_dir);        // NOTE** Non-default constructor hence must be defined with path immmediately rather than in a setup function later (could do this with a pointer instead to get around this)
        glm::vec3 outpost_origin = glm::vec3(0.0f, 4.0f, 0.0f);

        // Populate data structures made
        float spawn_dimensions[3] = {7.5f, 7.5f, 7.5f};             // float spawn_dimensions[3] = {float(screen_width), float(screen_height), float(screen_height)};

        glm::vec3 camera_origin = glm::vec3(0.0f, 0.0f, -25.0f);
        float camera_theta = 0.0f;
        float camera_phi = 0.0f;
        float camera_fov = 45.0f;

        float prior_t = glfwGetTime();   // Time on previous frame
        float delta_t = 0.016f;   // Time between rendered frames -> ~0.016 for 60fps (UPDATED every frame anyway, initialised as this to prevent delta_t=0.0f errors)

        Core_Manager(unsigned int screen_width, unsigned int screen_height);
        ~Core_Manager();
        void setup();
        void setup_shaders();
        void setup_buffers();
        void setup_models();
        void read_keypresses(GLFWwindow *window);
        void render(GLFWwindow *window);
        void render_entities(GLFWwindow *window, glm::mat4 view, glm::mat4 projection);
        void render_pings(GLFWwindow *window, glm::mat4 view, glm::mat4 projection);
        void render_terrain(GLFWwindow *window, glm::mat4 view, glm::mat4 projection);
        void render_outpost(GLFWwindow *window, glm::mat4 view, glm::mat4 projection);
        
    private:
        unsigned int VBO_entity;    // Entity buffers
        unsigned int VAO_entity;    //
        unsigned int EBO_entity;    //
        unsigned int VBO_ping;  // Ping sphere buffers
        unsigned int VAO_ping;  //
        unsigned int EBO_ping;  //
        unsigned int VBO_terrain;   // Terrain buffers
        unsigned int VAO_terrain;   //
        unsigned int EBO_terrain;   //

        void populate_creatures(Creature *creatures, const unsigned int creature_number, float *spawn_dimemsions);
        void populate_resources(Resource *resources, const unsigned int resource_number, float *spawn_dimemsions);

        int fetch_terrain_vertex_data_size(int x_cells, int z_cells);       // [X,Z,...] point for each vertex (Y given as a uniform)
        int fetch_terrain_vertex_indices_size(int x_cells, int z_cells);    // [I1, I2, I3,...] for each tri (3 indices), 2 tris per square grid, fence post problem for number of squares
        void generate_terrain_vertex_info(int x_cells, int z_cells, float cell_width, float *terrain_vertex_data, int *terrain_vertex_indices);
        void generate_terrain_coefficients(glm::mat4 *coefficient_matrix, int seed);
        float terrain_heightmap(float x, float z, glm::mat4 terrain_coefficients);
        void calculate_entities();
        float fetch_ping_signal_strength(float *pings, int ping_index, float time);
        float fetch_ping_radius(float *pings, int ping_index, float time);
        void add_ping(float *pings, int *ping_number, Entity *entity, float strength, float signal, float time);
        void remove_pings(float *pings, int *ping_number, float time);
        void evaluate_pings(float *pings, int *ping_number, Creature *creatures, const unsigned int creature_number, Resource *resources, const unsigned int resource_number, float time);
        void search_entities_spherical(Entity *entities, const unsigned int entity_number, glm::vec3 origin, float search_radius, int *found_entity_indices, int *found_entity_count, int *found_entity_limit, const int THREAD_NUM);
        void calculate_terrain_collision(Creature *creatures, const unsigned int creature_number, glm::mat4 terrain_coefficients, float terrain_width);
};

#endif
