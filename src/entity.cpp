#include "entity.h"

Entity::Entity() {
    // id = id;
    // position = iPos;
}

void Entity::directional_thrust(glm::vec3 udirection, float force) {
    /*
    . Applies a force on the entity in a specific direction
    . Applied to COM
    . Adds to the acceleration, but does not change velocity, etc, yet

    . udirection = unit vector direction
    */
    acceleration.x += force*udirection.x/mass;
    acceleration.y += force*udirection.y/mass;
    acceleration.z += force*udirection.z/mass;
}
void Entity::update_dynamics(float delta_t) {
    /*
    . Adds drag forces to the acceleration
    . Updates the velocity+position from the acceleration
    . Resets acceleration after this
    */
    // Drag forces
    // acceleration.x += ;

    // Update vel+pos
    velocity.x += acceleration.x;
    velocity.y += acceleration.y;
    velocity.z += acceleration.z;
    position.x += velocity.x*delta_t;
    position.y += velocity.y*delta_t;
    position.z += velocity.z*delta_t;

    // Reset acceleration
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
}