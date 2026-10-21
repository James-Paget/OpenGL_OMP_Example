#include "graphical_element.h"

Graphical_Element::Graphical_Element(glm::vec3 origin, glm::vec3 dimensions) {
    this->origin = origin;
    this->dimensions = dimensions;
}

void Graphical_Element::setup_full() {
    /*
    . Used for initial setup of a graphical element
    . Can be overridden by sub-classes to provide their own specific setup using sub-setup functions (e.g. setup_shader(), ...)
    */
}
void Graphical_Element::assign_shader() {
    /*
    . Applies its own shader used for rendering (NOT essential if another shader is known to match its vertex data)
    . Can be overridden by sub-classes to provide their own specific renders
    */
}
void Graphical_Element::bind_VAO() {
    /*
    . Binds the VAO data for this graphical element to allow for its rendering
    . Can be overridden by sub-classes to provide their own specific renders
    */
}
void Graphical_Element::render() {
    /*
    . Renders the element
    . Can be overridden by sub-classes to provide their own specific renders
    */
}
