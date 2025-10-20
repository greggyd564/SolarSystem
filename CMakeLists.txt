set(FETCHCONTENT_QUIET OFF)
cmake_minimum_required(VERSION 3.25)
project(CMakeSFMLProject LANGUAGES CXX)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    )
FetchContent_MakeAvailable(SFML)
message(STATUS "SFML_POPULATED = ${SFML_POPULATED}")

add_executable(main src/main.cpp src/Object.cpp src/cirQueue.cpp)
target_compile_features(main PRIVATE cxx_std_17)
#target_link_libraries(main PRIVATE SFML::Graphics)
target_link_libraries(main PRIVATE SFML::Graphics SFML::Window SFML::System)