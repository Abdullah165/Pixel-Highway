cmake_minimum_required(VERSION 4.0)
project(Pixel_Highway)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(RAYLIB_VERSION 5.5)

# Raylib: download it when building for web, or when it isn't installed
if (NOT "${PLATFORM}" STREQUAL "Web")
    find_package(raylib ${RAYLIB_VERSION} QUIET)
endif()

if (NOT raylib_FOUND)
    include(FetchContent)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
endif()

add_executable(${PROJECT_NAME} 
        src/main.cpp
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
        include/GameOver.h
        src/CrazySDK.cpp
        include/CrazySDK.h 
        src/PopupManager.cpp
        include/PopupManager.h
        include/TextUtils.h 
        include/ScreenShake.h 
        include/Difficulty.h 
        include/CoinManager.h
        src/CoinManager.cpp "include/Steering.h")

target_link_libraries(${PROJECT_NAME} raylib)

if ("${PLATFORM}" STREQUAL "Web")
    # Output index.html so CrazyGames can find it
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "index" SUFFIX ".html")

    # Re-link when shell.html changes
    set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_DEPENDS "${CMAKE_SOURCE_DIR}/shell.html")

    target_link_options(${PROJECT_NAME} PRIVATE
            -sUSE_GLFW=3
            -sASYNCIFY
            -sALLOW_MEMORY_GROWTH=1
            -sINITIAL_MEMORY=134217728
            -sSTACK_SIZE=5242880
            -sEXPORTED_RUNTIME_METHODS=ccall,HEAPF32,HEAP8,HEAPU8
            "--preload-file=${CMAKE_SOURCE_DIR}/assets@/assets"
            "--shell-file=${CMAKE_SOURCE_DIR}/shell.html")
else()
    # Desktop: copy the assets next to the exe
    file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
endif()