cmake_minimum_required(VERSION 3.10)

project(gameoflife)

set(SOURCE_DIR src)

file(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
