cmake_minimum_required(VERSION 4.0)
project(Pixel_Highway)

set(CMAKE_CXX_STANDARD 23)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 5.5)

# Look for Raylib and download if necessary
find_package(raylib ${RAYLIB_VERSION} QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_MakeAvailable(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    endif()
endif()

# Our Project
add_executable(${PROJECT_NAME} src/main.cpp
        src/CarController.cpp
        include/CarController.h
        src/Road.cpp
        include/Road.h
        src/NpcCarController.cpp
        include/NpcCarController.h
        src/SceneryController.cpp
        include/SceneryController.h
        src/Game.cpp
        include/Game.h
        src/SoundManager.cpp
        include/SoundManager.h
        src/ScoreManager.cpp
        include/ScoreManager.h
        src/GameOver.cpp
        include/GameOver.h)

# Copy the assets directory to the build output directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# Link our project against Raylib
target_link_libraries(${PROJECT_NAME} raylib)

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
endif()