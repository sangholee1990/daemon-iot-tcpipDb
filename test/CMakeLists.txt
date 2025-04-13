

cmake_minimum_required(VERSION 3.1)
project(sociService)

set(ABSL_PROPAGATE_CXX_STD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_executable(${CMAKE_PROJECT_NAME} sociService.cpp)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC /SYSTEMS/IOT/Roverdyn/PROJ_TCP_DB)
target_link_libraries(${CMAKE_PROJECT_NAME})

find_package(spdlog REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE spdlog::spdlog)

set(PRJ_PATH "/HDD/SYSTEMS/IOT/Roverdyn/PROJ_TCP_DB")
# add_definitions(-DPRJ_PATH=\"${PRJ_PATH}\")
add_compile_definitions(PRJ_PATH="${PRJ_PATH}")

# include(FindPkgConfig)
# pkg_check_modules(MySQL REQUIRED mysqlclient>=5.7)
# find_package(Threads REQUIRED)
# target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${MySQL_INCLUDE_DIRS})
# find_package(CURL REQUIRED)
# target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CURL_INCLUDE_DIRS})

# target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${MySQL_LIBRARIES} PRIVATE Threads::Threads PUBLIC ${CURL_LIBRARIES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(spdlog REQUIRED)
find_package(SOCI REQUIRED COMPONENTS mysql)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    Threads::Threads      # 스레드
    spdlog::spdlog        # spdlog
    SOCI::soci_core       # SOCI 코어 <<-- 추가됨
    SOCI::soci_mysql      # SOCI MySQL 백엔드 <<-- 추가됨
)