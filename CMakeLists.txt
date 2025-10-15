cmake_minimum_required(VERSION 3.29)
project(g2)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_PREFIX_PATH "/opt/homebrew")

find_package(SFML 3.0 REQUIRED COMPONENTS Graphics Window System)

add_executable(g2
        main.cpp
        Object.cpp
        Object.h
)

target_link_libraries(g2 PRIVATE SFML::Graphics SFML::Window SFML::System)

# Tell CLion about the SFML headers
target_include_directories(g2 PRIVATE ${SFML_INCLUDE_DIRS})
