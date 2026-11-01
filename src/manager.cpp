#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <omp.h>

#include "simulation_consts.h"
#include "custom_random.h"
#include "model.h"
#include "manager.h"

void manage_simulation_main(GLFWwindow *core_window, GLFWwindow *hud_window, unsigned int screen_width, const unsigned int screen_height) {
    /*
    . Defines the behaviour of the simulation logic and visuals
    . Separated from the core initialisation of the OpenGL
    */
    // Setup data structures
    glfwMakeContextCurrent(core_window);
    Core_Manager *_core_manager = new Core_Manager(screen_width, screen_height);
    _core_manager->setup();  // Initialises buffers and shaders

    glfwMakeContextCurrent(hud_window);
    HUD_Manager *_hud_manager = new HUD_Manager();
    _hud_manager->setup();   // Initialises buffers and shaders

    // Render
    glfwMakeContextCurrent(core_window);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Fill the face of drawn VAOs, not wireframe, and colour the backs too
    while( !glfwWindowShouldClose(core_window) && !glfwWindowShouldClose(hud_window) ) {
        refresh_hud(_hud_manager, _core_manager);
        
        _core_manager->render(core_window);
        _hud_manager->render(hud_window);

        // Once-per frame
        glfwPollEvents();
    }

    // Clean-up
    delete _core_manager;  // Perform clean up on window manager classes -> Delete buffers created
    delete _hud_manager;   //
}

void refresh_hud(HUD_Manager *_hud_manager, Core_Manager *_core_manager) {
    /*
    . Updates the statistics used by the HUD pulled from the Core window
    */
   float core_time = _core_manager->prior_t;
    if(core_time -_hud_manager->prior_refresh_time >= _hud_manager->refresh_rate) {
        _hud_manager->prior_refresh_time = core_time;

        // Collect data
        // (0)
        float bar_entityId_data[2] = {0,0};
        for(int i=0; i<_core_manager->creatures.size(); i++) {
            switch (_core_manager->creatures[i]->id) {
                case 0:
                    bar_entityId_data[0] = bar_entityId_data[0]+1;
                    break;
                case 1:
                    bar_entityId_data[1] = bar_entityId_data[1]+1;
                    break;
            }
        }

        // (1)
        float bar_randomTest_data[5] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

        // (2)
        dynamic_cast<GE_Linegraph*>(_hud_manager->graphical_elements[2])->data_x.push_back(core_time);
        dynamic_cast<GE_Linegraph*>(_hud_manager->graphical_elements[2])->data_y.push_back(5.0f*Custom_Random::rFloat());
        if( dynamic_cast<GE_Linegraph*>(_hud_manager->graphical_elements[2])->data_x.size() >= 10 ) {     //dynamic_cast<GE_Linegraph*>(_hud_manager->graphical_elements[2])->DATA_LIMIT_MAX
            dynamic_cast<GE_Linegraph*>(_hud_manager->graphical_elements[2])->data_x.erase(dynamic_cast<GE_Linegraph*>(_hud_manager->graphical_elements[2])->data_x.begin());
            dynamic_cast<GE_Linegraph*>(_hud_manager->graphical_elements[2])->data_y.erase(dynamic_cast<GE_Linegraph*>(_hud_manager->graphical_elements[2])->data_y.begin());
        }
        dynamic_cast<GE_Linegraph*>(_hud_manager->graphical_elements[2])->x_scale_end = core_time;
        dynamic_cast<GE_Linegraph*>(_hud_manager->graphical_elements[2])->x_scale_start = dynamic_cast<GE_Linegraph*>(_hud_manager->graphical_elements[2])->data_x[0];

        // Send data
        // NOTE*; Each chart needs specific tailored data, hence must be manually assigned for each => assume knowledge of which index is which sub-class type while casting
        dynamic_cast<GE_Bar*>(_hud_manager->graphical_elements[0])->refresh_data(bar_entityId_data, 2); // Bar - Entity IDs
        dynamic_cast<GE_Bar*>(_hud_manager->graphical_elements[1])->refresh_data(bar_randomTest_data, 5); // Bar - ...
        dynamic_cast<GE_Linegraph*>(_hud_manager->graphical_elements[2])->refresh_data(nullptr, nullptr, 0); // Linegraph - ...
    }
}
