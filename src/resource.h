#ifndef RESOURCE_H
#define RESOURCE_H

#include "glm/glm.hpp"
#include "entity.h"

class Resource : public Entity {
    public:
        uint resource_type = 0;

        Resource();

    private:
        //pass
};

#endif