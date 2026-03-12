cmake_minimum_required(VERSION 3.11)
project(sandbox)

set(CMAKE_CXX_STANDARD 20)

if (WIN32)
    include(C:/vcpkg/scripts/buildsystems/vcpkg.cmake)
endif()

file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

find_package(raylib CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

target_include_directories(${PROJECT_NAME} PRIVATE src)
file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})