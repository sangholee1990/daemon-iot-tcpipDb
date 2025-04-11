cmake_minimum_required(VERSION 3.1)
project(jsonReader)

set(ABSL_PROPAGATE_CXX_STD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


add_executable(${CMAKE_PROJECT_NAME} jsonReader.cpp)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC /SYSTEMS/IOT/Roverdyn/PROJ_TCP_DB)

target_link_libraries(${CMAKE_PROJECT_NAME})

set(PRJ_PATH "/HDD/SYSTEMS/IOT/Roverdyn/PROJ_TCP_DB")
add_definitions(-DPRJ_PATH=\"${PRJ_PATH}\")