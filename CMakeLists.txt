cmake_minimum_required(VERSION 3.5.0)
project(opengl_omp_sample_program VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)
find_package(OpenMP REQUIRED)
find_package(MPI REQUIRED)

add_executable(opengl_omp_sample_program 
    src/main.cpp
    src/glad.h
    src/glad.c
    src/manager.h
    src/manager.cpp
    src/entity.h
    src/entity.cpp
    src/resource.h
    src/resource.cpp
    src/creature.h
    src/creature.cpp
    src/glm/glm.hpp
    src/glm/gtc/matrix_transform.hpp
    src/glm/gtc/type_ptr.hpp
)

target_link_libraries(opengl_omp_sample_program 
    glfw
    OpenGL::GL
    OpenMP::OpenMP_CXX
    MPI::MPI_CXX
)