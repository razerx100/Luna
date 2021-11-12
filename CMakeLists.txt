cmake_minimum_required(VERSION 3.21)

project(Luna
    LANGUAGES CXX
)

set(PROJECTDIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
else()
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/")
endif()

if(IMGUIFLAG STREQUAL "ENABLE")
    file(GLOB_RECURSE SRC src/*.cpp includes/*.hpp exports/*.hpp templates/*.hpp ImGui/*.cpp ImGui/*.h)
else()
    file(GLOB_RECURSE SRC src/*.cpp includes/*.hpp exports/*.hpp templates/*.hpp)
endif()

add_library(${PROJECT_NAME} SHARED
    ${SRC}
)

target_include_directories(${PROJECT_NAME} PRIVATE includes/ exports/ includes/Exceptions/ templates/)

target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:_DEBUG>" "$<$<CONFIG:RELEASE>:NDEBUG>" BUILD_LUNA)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Xinput.lib
)

if(IMGUIFLAG STREQUAL "ENABLE")
    target_include_directories(${PROJECT_NAME} PRIVATE ImGui/includes/)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _IMGUI)
endif()

if(MSVC)
    string(REPLACE "/W3 " "/W4 " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    string(REPLACE "/W3 " "/W4 " CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    string(REPLACE "/W3 " "/W4 " CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /fp:fast /MP /EHa /Ot)
endif()

include(FetchContent)

FetchContent_Declare(
    ${INPUT_MANAGER}
    GIT_REPOSITORY https://github.com/razerx100/Pluto.git
    GIT_TAG master
)

FetchContent_MakeAvailable(${INPUT_MANAGER})

if(GRAPHICS_ENGINE STREQUAL "GaiaX")
    FetchContent_Declare(
        ${GRAPHICS_ENGINE}
        GIT_REPOSITORY https://github.com/razerx100/GaiaX.git
        GIT_TAG master
    )

    FetchContent_MakeAvailable(${GRAPHICS_ENGINE})
elseif(GRAPHICS_ENGINE STREQUAL "Terra")
    FetchContent_Declare(
        ${GRAPHICS_ENGINE}
        GIT_REPOSITORY https://github.com/razerx100/Terra.git
        GIT_TAG master
    )

    FetchContent_MakeAvailable(${GRAPHICS_ENGINE})
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${CMAKE_BINARY_DIR}/lib/$<$<CONFIG:DEBUG>:Debug>$<$<CONFIG:RELEASE>:Release>/InputManager.lib
    ${CMAKE_BINARY_DIR}/lib/$<$<CONFIG:DEBUG>:Debug>$<$<CONFIG:RELEASE>:Release>/GraphicsEngine.lib
)

add_dependencies(${PROJECT_NAME} ${INPUT_MANAGER} ${GRAPHICS_ENGINE})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "RenderWindow")

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
