#include "entity.h"
#include "creature.h"
#include "resource.h"
#include "GLFW/glfw3.h"

#include "hud_manager.h"

#ifndef MANAGER_H
#define MANAGER_H

void manage_simulation_main(GLFWwindow *core_window, GLFWwindow *hud_window, const unsigned int screen_width, const unsigned int screen_height);

void calculate_entities(float *pings, int *ping_number, float *ping_sphere_vertex_data, int *ping_sphere_vertex_indices, glm::mat4 terrain_coefficients, float terrain_width, Creature *creatures, const unsigned int creature_number, Resource *resources, const unsigned int resource_number, float *prior_t);
void render_core(GLFWwindow *core_window, unsigned int shaderProgram, unsigned int shaderProgram_pingSphere, unsigned int shaderProgram_terrain, unsigned int VAO_entity, unsigned int VAO_ping_sphere, unsigned int VAO_terrain, float *pings, int *ping_number, float *ping_sphere_vertex_data, int *ping_sphere_vertex_indices, int *terrain_x_cells, int *terrain_z_cells, glm::mat4 terrain_coefficients, Creature *creatures, const unsigned int creature_number, Resource *resources, const unsigned int resource_number, glm::vec3 *camera_origin, float *camera_theta, float *camera_phi, float *camera_fov, const unsigned int screen_width, const unsigned int screen_height, float *prior_t, float *delta_t);
void read_keypress_core(GLFWwindow *window, float *pings, int *ping_number, Creature *creatures);
void render_hud(GLFWwindow *hud_window, unsigned int shaderProgram, unsigned int VAO_entity, Creature *creatures, const unsigned int creature_number, const unsigned int screen_width, const unsigned int screen_height);

void populate_creatures(Creature *creatures, const unsigned int creature_number, float *spawn_dimemsion);
void populate_resources(Resource *resources, const unsigned int resource_number, float *spawn_dimemsions);
float fetch_ping_signal_strength(float *pings, int ping_index, float time);
float fetch_ping_radius(float *pings, int ping_index, float time);
void add_ping(float *pings, int *ping_number, Entity *entity, float strength, float signal, float time);
void remove_pings(float *pings, int *ping_number, float time);
void evaluate_pings(float *pings, int *ping_number, Creature *creatures, const unsigned int creature_number, Resource *resources, const unsigned int resource_number, float time);
void search_entities_spherical(Entity *entities, const unsigned int entity_number, glm::vec3 origin, float search_radius, int *found_entity_indices, int *found_entity_count, int *found_entity_limit, const int THREAD_NUM=4);

void calculate_terrain_collision(Creature *creatures, const unsigned int creature_number, glm::mat4 terrain_coefficients, float terrain_width);

int fetch_terrain_vertex_data_size(int x_cells, int z_cells);
int fetch_terrain_vertex_indices_size(int x_cells, int z_cells);
void generate_terrain_vertex_info(int x_cells, int z_cells, float cell_width, float *terrain_vertex_data, int *terrain_vertex_indices);
void generate_terrain_coefficients(int matrix_width, glm::mat4 *coefficient_matrix, int seed);
float terrain_heightmap(float x, float z, glm::mat4 terrain_coefficients);

void refresh_hud(HUD_Manager *_hud_manager, const unsigned int creature_number, Creature *creatures, float time);

float rand_float(int seed_offset);

#endif
