#ifndef CREATURE_H
#define CREATURE_H

#include "glm/glm.hpp"
#include "entity.h"

class Creature : public Entity {
    public:
        uint creature_type = 0;

        /*
        -- BEHAVIOURS --
        0 = Idle/wander
        1 = Travel towards last ping
        ...

        --> Behaviours & details set as pings are evaluated
        */
        uint behaviour = 0;
        int prior_ping_signal = 0;                          // Details about last relevant ping heard (external pings)
        glm::vec3 prior_ping_direction = glm::vec3(0.0f);   //  -> relevant ping meaning it was heard and certain conditions were met too
        
        glm::vec3 memory_vector3 = glm::vec3(0.0f);         // Generic location stored that caries various meanings depending on which behaviour is in play
        
        float travel_speed = 0.0005f;

        bool ping_toggle = true;   // Switches between true and false to trigger single-instant ping releases

        Creature();

    private:
        //pass
};

#endif