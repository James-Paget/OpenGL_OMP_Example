#include <iostream>

#include "custom_random.h"

float Custom_Random::rFloat() {
    /*
    . Return random float in range (0,1]
    */
    float r1 = rand();
    float r2 = rand();
    return (r1<r2) ? r1/r2 : r2/r1;
}
