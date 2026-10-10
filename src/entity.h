#include "glm/glm.hpp"

#ifndef ENTITY_H
#define ENTITY_H

class Entity {
    public:
        uint id = 0;

        float mass = 1.0f;

        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

        glm::vec3 forward_direction = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 up_direction = glm::vec3(0.0f, 1.0f, 0.0f);

        float ping_cooldown = 4.0f;     // Minimum time between pings from this entity
        float prev_ping_time = -ping_cooldown;    // When last ping occured (GLFW time)

        Entity();

        void directional_thrust(glm::vec3 udirection, float force);
        void update_dynamics(float delta_t);
        void ping(float *ping_set, int *ping_counter, float current_time, float strength, float signal);

    private:
        //pass
};

#endif
