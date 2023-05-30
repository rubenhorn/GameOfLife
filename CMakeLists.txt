cmake_minimum_required(VERSION 3.10)

project(gameoflife)

set(SOURCE_DIR src)

file(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

# Add OpenMP support
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

add_executable(${PROJECT_NAME} ${SOURCES})
