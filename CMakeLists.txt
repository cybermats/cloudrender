cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)
include(CMakeToolsHelpers OPTIONAL)

project(cloudrender)

set(SOURCE "source/main.cpp")
add_executable("${PROJECT_NAME}" ${SOURCE})
