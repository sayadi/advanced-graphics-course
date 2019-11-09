cmake_minimum_required(VERSION 3.14)
project(AdvancedCG_Assignment2)

set(CMAKE_CXX_STANDARD 14)

add_executable(
    AdvancedCG_Assignment2
#    template.cpp
#    hw1.cpp
    hw2.cpp
)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries( AdvancedCG_Assignment2 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
