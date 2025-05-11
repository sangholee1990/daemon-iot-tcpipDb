cmake_minimum_required(VERSION 3.1)
project(SOLARMY_APP VERSION 1.0.0)

# 전역 설정
set(ABSL_PROPAGATE_CXX_STD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# 실행 파일 정의
add_executable(${CMAKE_PROJECT_NAME} main.cpp MySQLService.cpp TCPService.cpp)

# 의존성 라이브러리 찾기
include(FindPkgConfig)

# find_package(MySQL REQUIRED)
pkg_check_modules(MYSQL REQUIRED mysqlclient>=5.7)
if(MYSQL_FOUND)
    message(STATUS "MYSQL_INCLUDE_DIRS (pkg-config): ${MYSQL_INCLUDE_DIRS}")
    message(STATUS "MYSQL_LIBRARIES (pkg-config): ${MYSQL_LIBRARIES}")
    message(STATUS "MYSQL_LIBRARY_DIRS (pkg-config): ${MYSQL_LIBRARY_DIRS}")
    message(STATUS "MYSQL_LINK_LIBRARIES (pkg-config): ${MYSQL_LINK_LIBRARIES}")
    
    target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${MYSQL_INCLUDE_DIRS})
    target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${MYSQL_LIBRARY_DIRS})
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${MYSQL_LINK_LIBRARIES})
else()
    message(FATAL_ERROR "pkg-config could not find mysqlclient (version >= 5.7).")
endif()

find_package(Threads REQUIRED)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${MySQL_INCLUDE_DIRS})

find_package(CURL REQUIRED)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CURL_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${CURL_LIBRARIES})

find_package(spdlog REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE spdlog::spdlog)

# 컴파일 정의
set(PRJ_PATH "/HDD/SYSTEMS/IOT/Roverdyn/daemon-iot-tcpipDb")
add_compile_definitions(PRJ_PATH="${PRJ_PATH}")
