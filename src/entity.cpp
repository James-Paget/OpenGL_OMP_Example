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
void Entity::ping(float *ping_set, int *ping_counter, float current_time, float strength, float signal) {
    /*
    . Sends a ping to the ping_set list of pings
    . This will be removed (by another function) from the set once it has been active for more than PING_SIGNAL_TIME_MAX duration
    . All pings travel at the same speed

    . ping_counter = index where this ping starts; e.g. 6*ping_number
    . current_time = current glfwGetTime reading
    . strength = Starting strength for signal sent; decays over time, must be above a cerrain strength to be reflected by other entities
    . signal = The type of ping sent; is fixed
    */
    ping_set[(*ping_counter)*6+0] = position.x;
    ping_set[(*ping_counter)*6+1] = position.y;
    ping_set[(*ping_counter)*6+2] = position.z;
    ping_set[(*ping_counter)*6+3] = current_time;
    ping_set[(*ping_counter)*6+4] = strength;
    ping_set[(*ping_counter)*6+5] = signal;
    *ping_counter++;
}