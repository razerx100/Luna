cmake_minimum_required(VERSION 3.21)

file(GLOB_RECURSE SRC src/*.cpp src/*.ixx includes/*.hpp templates/*.hpp exports/*.hpp)

if(ADD_TEST_LUNA)
    set(LIBRARY_TYPE STATIC)
else()
    set(LIBRARY_TYPE SHARED)
endif()

add_library(LunaLib ${LIBRARY_TYPE}
    ${SRC}
)

set(LUNA_PRIVATE_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/includes/ ${CMAKE_CURRENT_SOURCE_DIR}/includes/Exceptions/ ${CMAKE_CURRENT_SOURCE_DIR}/templates/)

set(LUNA_PRIVATE_INCLUDES ${LUNA_PRIVATE_INCLUDES} PARENT_SCOPE)

target_include_directories(LunaLib PRIVATE ${LUNA_PRIVATE_INCLUDES})

target_include_directories(LunaLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/exports/)

target_compile_definitions(LunaLib PRIVATE BUILD_LUNA)

target_link_libraries(LunaLib PRIVATE
    Xinput.lib
)

target_compile_options(LunaLib PRIVATE /fp:fast /MP /Ot /W4 /Gy)
