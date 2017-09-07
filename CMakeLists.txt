cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)
include(CMakeToolsHelpers OPTIONAL)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
  ${CMAKE_BINARY_DIR}/googletest-build)

project(cloudrender CXX C)

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_BUILD_TYPE Debug)
find_package (Threads REQUIRED)
find_package (Boost COMPONENTS program_options system REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

set(SOURCE
  source/scene.cpp
  source/camera.cpp
  source/vecutil.cpp
  source/config.cpp
  source/radiance_sample.cpp
  source/ray.cpp
  source/color.cpp
  source/triangle.cpp
  source/obj_file.cpp
  )

set(LIBRARIES
  Threads::Threads
  ${Boost_LIBRARIES}
  )

add_executable("${PROJECT_NAME}" source/main.cpp ${SOURCE})
add_executable("unittest_${PROJECT_NAME}" source/gtestmain.cpp ${SOURCE})
target_link_libraries ("${PROJECT_NAME}" ${LIBRARIES})
target_link_libraries ("unittest_${PROJECT_NAME}" ${LIBRARIES} gtest_main)
enable_testing()
add_test(NAME cloud_test COMMAND "unittest_${PROJECT_NAME}")
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
