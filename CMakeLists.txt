cmake_minimum_required(VERSION 3.1)
project(SOLARMY_APP VERSION 1.0.0)

set(ABSL_PROPAGATE_CXX_STD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_executable(${CMAKE_PROJECT_NAME} main.cpp MySQLService.cpp TCPService.cpp)

include(FindPkgConfig)
pkg_check_modules(MySQL REQUIRED mysqlclient>=5.7)
find_package(Threads REQUIRED)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${MySQL_INCLUDE_DIRS})
find_package(CURL REQUIRED)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CURL_INCLUDE_DIRS})

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${MySQL_LIBRARIES} PRIVATE Threads::Threads PUBLIC ${CURL_LIBRARIES})

find_package(spdlog REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE spdlog::spdlog)

set(PRJ_PATH "/HDD/SYSTEMS/IOT/Roverdyn/PROJ_TCP_DB")
add_definitions(-DPRJ_PATH=\"${PRJ_PATH}\")
