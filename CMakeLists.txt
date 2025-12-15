cmake_minimum_required(VERSION 3.20)
project(MyEngine C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source files
file(GLOB_RECURSE ENGINE_SRC
    src/*.c
)

add_library(engine STATIC ${ENGINE_SRC})

target_include_directories(engine PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Platform specifics
if (WIN32)
    target_compile_definitions(engine PRIVATE PLATFORM_WINDOWS)
endif()

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(engine PUBLIC OpenGL::GL)
