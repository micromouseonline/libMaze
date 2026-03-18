cmake_minimum_required(VERSION 3.18)
project(libMaze-tests CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# On Windows with MSVC, ensure GTest uses the same CRT as the test binaries.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

set(LIBMAZE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Only include libMaze sources needed by the current tests.
# Add further files here as tests require them.
set(LIBMAZE_SOURCES
        ${LIBMAZE_DIR}/compiler.cpp
        ${LIBMAZE_DIR}/maze.cpp
        ${LIBMAZE_DIR}/mazedata.cpp
        ${LIBMAZE_DIR}/mazefiler.cpp
        ${LIBMAZE_DIR}/mazepathfinder.cpp
        ${LIBMAZE_DIR}/mazeprinter.cpp
        ${LIBMAZE_DIR}/mazesearcher.cpp
)

add_executable(maze_tests
        00-test-blank.cpp
        01-direction.cpp
        02-priority-queue.cpp
        03-maze-16-tests.cpp
        04-direction-to-smallest-test.cpp
        05-util.cpp
        06-compiler.cpp
        07-maze-extended.cpp
        08-mazeprinter.cpp
        09-mazefiler.cpp
        10-pathfinder.cpp
        11-mazesearcher.cpp
        path-test-data.cpp
        ${LIBMAZE_SOURCES}
)

target_include_directories(maze_tests PRIVATE
        ${LIBMAZE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../src
)

target_compile_definitions(maze_tests PRIVATE ENABLE_MAZE_DATA)

target_link_libraries(maze_tests PRIVATE
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(maze_tests DISCOVERY_MODE PRE_TEST)
