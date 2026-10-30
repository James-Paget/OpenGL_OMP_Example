#ifndef CREATURE_H
#define CREATURE_H

#include "glm/glm.hpp"
#include "entity.h"

class Creature : public Entity {
    public:
        uint creature_type = 0;

        Creature();

    private:
        //pass
};

#endif