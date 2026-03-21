cmake_minimum_required(VERSION 3.12)

if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(maze VERSION 18.09.13.2)


project(maze)

set(HEADER_FILES
        commandnames.h
        maze.h
        mazeconstants.h
        mazedata.h
        mazepathfinder.h
        mazeprinter.h
        mazesearcher.h
        priorityqueue.h
        mazeconstants.h
        mazefiler.h
        floodinfo.h
        )

set(SOURCE_FILES
        maze.cpp
        mazedata.cpp
        mazepathfinder.cpp
        mazeprinter.cpp
        mazefiler.cpp
        mazesearcher.cpp
        compiler.cpp
        compiler.h
        )

add_library(maze
        STATIC
        ${SOURCE_FILES}
        ${HEADER_FILES}
        )

# Allow parent project to control via -DENABLE_MAZE_DATA or set() before add_subdirectory
if(ENABLE_MAZE_DATA)
  target_compile_definitions(maze PUBLIC ENABLE_MAZE_DATA)
endif()