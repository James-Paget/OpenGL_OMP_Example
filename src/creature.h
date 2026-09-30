#include "glm/glm.hpp"
#include "entity.h"

#ifndef CREATURE_H
#define CREATURE_H

class Creature : public Entity {
    public:
        uint creature_type = 0;

        Creature();

    private:
        //pass
};

#endif