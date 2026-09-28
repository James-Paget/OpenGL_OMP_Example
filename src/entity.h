#include "glm/glm.hpp"

#ifndef ENTITY_H
#define ENTITY_H

class Entity {
    public:
        uint id = 0;
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
        Entity();

    private:
        //pass
};

#endif