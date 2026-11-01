#ifndef PING_H
#define PING_H

#include "glm/glm.hpp"

struct Ping {
    glm::vec3 position;
    float time, strength;
    int signal;
};

#endif
