cmake_minimum_required(VERSION 3.5)
project(ic3)
find_package(Z3 REQUIRED CONFIG)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Z3    FOUND: ${Z3_FOUND} | VERSION: ${Z3_VERSION_STRING} | DIR: ${Z3_DIR}")

add_compile_definitions("DEBUG=$<CONFIG:Debug>")
add_compile_options(-O3)
add_executable(ic3
        aiger/aiger.c
        aigToZ3.cpp
        ic3.cpp
        main.cpp)
target_include_directories(ic3 PRIVATE ${Z3_CXX_INCLUDE_DIRS})
target_link_libraries(ic3 PRIVATE ${Z3_LIBRARIES})
target_compile_options(ic3 PRIVATE ${Z3_COMPONENT_CXX_FLAGS})

add_executable(aigerToDot aiger/aigtodot.c aiger/aiger.c)
