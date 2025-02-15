cmake_minimum_required(VERSION 3.21)

file(GLOB_RECURSE SRC src/*.cc)

add_executable(LunaTest
    ${SRC}
)

target_include_directories(LunaTest PRIVATE ${LUNA_PRIVATE_INCLUDES})

unset(LUNA_PRIVATE_INCLUDES)

target_compile_options(LunaTest PRIVATE /fp:fast /MP /Ot /W4 /Gy /std:c++20 /Zc:__cplusplus)

include(FetchContent)

set(GOOGLE_TEST_COMMIT_ID 12a5852e451baabc79c63a86c634912c563d57bc CACHE STRING "Supply the latest commit ID from the GitHub repository.")

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/${GOOGLE_TEST_COMMIT_ID}.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

target_link_libraries(LunaTest PRIVATE
    GTest::gtest_main LunaLib
)

include(GoogleTest)

gtest_discover_tests(LunaTest)
