cmake_minimum_required(VERSION 3.14)
project(advanced-graphics-course)

set(CMAKE_CXX_STANDARD 14)

add_executable(
    advanced-graphics-course
#    template.cpp
#    hw1.cpp
#    hw2.cpp
    textfile.cpp
#    hw3.cpp
     hw4.cpp
)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} )

target_link_libraries( advanced-graphics-course ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY} )
