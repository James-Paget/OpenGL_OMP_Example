#include "simple_triangle_example.h"

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

void perform_simple_triangle_example(GLFWwindow* window) {
    /*
    . EXAMPLE FROM https://learnopengl.com/Getting-started/Hello-Triangle

    . Full program for the simple triangle example program
    . Initialises data and performs the draw loop
    . Assumes GLFW and GLAD already initialised AND the window created

    . Vertex stage -> geometry "" -> primitives "" -> rasterise "" -> fragment shader "" -> 'blending' ""
                                                         /\ Clipped here (remove pixels outside view)
    . Fragment = All data needed for a pixel (colour, pos, ...)
    . Blending stage = Depth (is pixel in front or behind others)
    . VBO = Vertex buffer object => Stores large number of vertices on GPU memory -> Sends large block not one at a time (slow-process otherwise)
    . VAO = Vertex array object => Binds vertex attributes as well, meaning they do not need to be rebound each draw step -> Just bind the VAO each draw step
    . EBO = Element buffer object => Stores (unique) vertices AND indices pointing to them to specify shapes to draw, preventing the need for doubles of indices
    */
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    }; 
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // *** Vertex shader: Handles geometry data (positions, normals, etc) -> Important for lighting
    // *** Fragment shader: Handles colour data (pixel colours, shades, etc)
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // int  success;
    // char infoLog[512];
    // glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // *** Shader program: Links together inputs and outputs of shaders within stage cycles -> This is actually used at the end for the full graphics process
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);       // Save memory by removing these AFTER linking
    glDeleteShader(fragmentShader);     // "" ""
    // **NOTE; Has a SUCCESS log available as well [512 chars using glGetProgramiv & glGetProgramInfoLog]

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);  
    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    unsigned int VBO;                                                                   // Create ID space for new VBO buffer
    glGenBuffers(1, &VBO);                                                              // Generate an arbitrary buffer (many diff buffers available) with a corresponding ID
    glBindBuffer(GL_ARRAY_BUFFER, VBO);                                                 // Link the ID to the buffer with buffer type GL_ARRAY_BUFFER --> NOTE* Only allowed one buffer of each type bound at once I believe (### DOUBLE CHECK THIS ###)
    // Now it is bound, any changes to the unique buffer (one of a kind) GL_ARRAY_BUFFER, will affect the VBO ID'ed list
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);          // Copies the data to the GL_ARRAY_BUFFER buffer (unique) which is currently bound to VBO
    // 3. copy our index array in a element buffer for OpenGL to use
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);     // **NOTE; When a VAO is bound, binding the EBO automatically places the EBO within the VAO; Hence the VAO remembers the EBO given + rebinding the VAO auto does the EBO too
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);   // layout(position=0) => aPos tagged with 0 index, vec3 => 3 floats long, stride given after (3 floats long for vec3), final param is the offset of where the data starts (but requires void* type hence the cast)
    // GL_TRUE => (Int/Bytes used for data type, unnormalised)
    // Pulls vertices from BOUND VBO
    glEnableVertexAttribArray(0);   // Actually populates this vertex attribute info for each vertex

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // WIREFRAME MODE

    while(!glfwWindowShouldClose(window)) {     // Loop while the specified window is open
        glfwPollEvents();

        // ..:: Drawing code (in render loop) :: ..
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);    // FOR VAOs: Draw TRIs, start at vertex 0, go to vertex 3 (for a vertex array of 3 vertices => 9 elements)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);    // FOR EBOs: Draw mode, vertex number (total => 4 unique but 2 triangles hence 6 total vertices), then the type of indices (+ve ints => unsigned ints), and finally the offset to where to start
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }
}

void perform_simple_triangle_example_ex1(GLFWwindow* window) {
    /*
    . EXAMPLE FROM https://learnopengl.com/Getting-started/Hello-Triangle

    . Full program for the simple triangle example program
    . Initialises data and performs the draw loop
    . Assumes GLFW and GLAD already initialised AND the window created

    . Vertex stage -> geometry "" -> primitives "" -> rasterise "" -> fragment shader "" -> 'blending' ""
                                                         /\ Clipped here (remove pixels outside view)
    . Fragment = All data needed for a pixel (colour, pos, ...)
    . Blending stage = Depth (is pixel in front or behind others)
    . VBO = Vertex buffer object => Stores large number of vertices on GPU memory -> Sends large block not one at a time (slow-process otherwise)
    . VAO = Vertex array object => Binds vertex attributes as well, meaning they do not need to be rebound each draw step -> Just bind the VAO each draw step
    . EBO = Element buffer object => Stores (unique) vertices AND indices pointing to them to specify shapes to draw, preventing the need for doubles of indices
    */
    float vertices[] = {
        0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };
    unsigned int indices[] = {  // note that we start from 0!
        0,1,2,
        0,3,4
    }; 
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // *** Vertex shader: Handles geometry data (positions, normals, etc) -> Important for lighting
    // *** Fragment shader: Handles colour data (pixel colours, shades, etc)
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);       // Save memory by removing these AFTER linking
    glDeleteShader(fragmentShader);     // "" ""

    unsigned int VBO;                                                                   // Create ID space for new VBO buffer
    glGenBuffers(1, &VBO);                                                              // Generate an arbitrary buffer (many diff buffers available) with a corresponding ID
    glBindBuffer(GL_ARRAY_BUFFER, VBO);                                                 // Link the ID to the buffer with buffer type GL_ARRAY_BUFFER --> NOTE* Only allowed one buffer of each type bound at once I believe (### DOUBLE CHECK THIS ###)
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);          // Copies the data to the GL_ARRAY_BUFFER buffer (unique) which is currently bound to VBO

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);  
    glBindVertexArray(VAO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);     // **NOTE; When a VAO is bound, binding the EBO automatically places the EBO within the VAO; Hence the VAO remembers the EBO given + rebinding the VAO auto does the EBO too
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);   // layout(position=0) => aPos tagged with 0 index, vec3 => 3 floats long, stride given after (3 floats long for vec3), final param is the offset of where the data starts (but requires void* type hence the cast)
    glEnableVertexAttribArray(0);   // Actually populates this vertex attribute info for each vertex

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // WIREFRAME MODE
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // FILLED MODE

    while(!glfwWindowShouldClose(window)) {     // Loop while the specified window is open
        glfwPollEvents();

        // ..:: Drawing code (in render loop) :: ..
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);    // FOR VAOs: Draw TRIs, start at vertex 0, go to vertex 3 (for a vertex array of 3 vertices => 9 elements)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);    // FOR EBOs: Draw mode, vertex number (total => 4 unique but 2 triangles hence 6 total vertices), then the type of indices (+ve ints => unsigned ints), and finally the offset to where to start
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }
}

void perform_simple_triangle_example_ex2(GLFWwindow* window) {
    /*
    . EXAMPLE FROM https://learnopengl.com/Getting-started/Hello-Triangle

    . Full program for the simple triangle example program
    . Initialises data and performs the draw loop
    . Assumes GLFW and GLAD already initialised AND the window created

    . Vertex stage -> geometry "" -> primitives "" -> rasterise "" -> fragment shader "" -> 'blending' ""
                                                         /\ Clipped here (remove pixels outside view)
    . Fragment = All data needed for a pixel (colour, pos, ...)
    . Blending stage = Depth (is pixel in front or behind others)
    . VBO = Vertex buffer object => Stores large number of vertices on GPU memory -> Sends large block not one at a time (slow-process otherwise)
    . VAO = Vertex array object => Binds vertex attributes as well, meaning they do not need to be rebound each draw step -> Just bind the VAO each draw step
    . EBO = Element buffer object => Stores (unique) vertices AND indices pointing to them to specify shapes to draw, preventing the need for doubles of indices
    */
    float vertices_A[] = {
        0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f
    };
    float vertices_B[] = {
        0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f
    };
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // *** Vertex shader: Handles geometry data (positions, normals, etc) -> Important for lighting
    // *** Fragment shader: Handles colour data (pixel colours, shades, etc)
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);       // Save memory by removing these AFTER linking
    glDeleteShader(fragmentShader);     // "" ""


    unsigned int VBO_A;
    glGenBuffers(1, &VBO_A);
    unsigned int VBO_B;
    glGenBuffers(1, &VBO_B);


    unsigned int VAO_A;
    glGenVertexArrays(1, &VAO_A);
    glBindVertexArray(VAO_A);
    unsigned int VAO_B;
    glGenVertexArrays(1, &VAO_B);
    glBindVertexArray(VAO_B);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // WIREFRAME MODE
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // FILLED MODE

    while(!glfwWindowShouldClose(window)) {     // Loop while the specified window is open
        glfwPollEvents();

        glUseProgram(shaderProgram);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_A);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_A), vertices_A, GL_STATIC_DRAW);
        glBindVertexArray(VAO_A);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_B);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_B), vertices_B, GL_STATIC_DRAW);
        glBindVertexArray(VAO_B);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }
}

// 16 4-component (e.g. vectors) vertex attributes allowed