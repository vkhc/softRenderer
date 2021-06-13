cmake_minimum_required(VERSION 3.12)

set(PROJECT renderer)

project(${PROJECT} VERSION 1.2.3 LANGUAGES CXX)


list(APPEND CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/cmake/sdl2/)
find_package(SDL2 REQUIRED)

# Global CMake variables
set(CMAKE_CXX_STANDARD               17)
set(CMAKE_CXX_STANDARD_REQUIRED      ON)
set(CMAKE_CXX_EXTENSIONS            OFF)

# Project and libbrary names
set(MAIN         "${PROJECT}_Executable")
set(TEST_MAIN    "${PROJECT}_unitTests")
set(LIB_NAME     mylib)


# Building and generating options
option(ENABLE_TESTING "Enable unit testing build." ON)

# cmake modules.
if (ENABLE_TESTING)
    include(CTest)
endif()

# Add libraries
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

# Add third party libraries
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdParty)

# Add executables
add_subdirectory(${PROJECT_SOURCE_DIR}/main)



# Add tests
if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif()