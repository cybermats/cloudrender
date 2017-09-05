cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)
include(CMakeToolsHelpers OPTIONAL)

project(cloudrender CXX C)

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_BUILD_TYPE Debug)
find_package (Threads REQUIRED)
find_package (Boost COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})


set(SOURCE "source/main.cpp")
add_executable("${PROJECT_NAME}" ${SOURCE})
target_link_libraries (cloudrender Threads::Threads ${Boost_LIBRARIES})
