cmake_minimum_required(VERSION 3.21)

file(GLOB_RECURSE SRC src/*.cpp src/*.ixx includes/*.hpp templates/*.hpp exports/*.hpp)

add_library(LunaLib STATIC ${SRC})

target_include_directories(LunaLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/exports/ ${CMAKE_CURRENT_SOURCE_DIR}/includes/)

target_compile_options(LunaLib PRIVATE /fp:fast /MP /Ot /W4 /Gy /std:c++20 /Zc:__cplusplus)
