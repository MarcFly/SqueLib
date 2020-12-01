# Set bae CMake values
cmake_minimum_required(VERSION 3.1...3.18) #require a minimum cmake version to build
# If the version is less than tested, make a warning about it, else try to use 3.18
if(${CMAKE_VERSION} VERSION_LESS 3.18)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.18)
endif()

project(FlyLib  VERSION 2020.1
                    DESCRIPTION "Multiplatform C++ Library for making games"
                    LANGUAGES CXX)
set(FlyLib_Modules ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)
# Set Naming Vars
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON) # For Android have to be turned off

# Include External Modules for helper functions
include(${FlyLib_Modules}/External/Helpers.cmake)

# Things that are shared between all builds
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/flylib_src)
execute_process(COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/builds)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/builds)

# Do a big ifdef for specific parts of the library
# First if if full library define
# Ifdefs for not using parts, for now full build

file(GLOB_RECURSE FLYLIB_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/flylib_src/*.cpp")

# Include modules for creating builds to diferent platforms
# Requires file per platform to platform
# I prefer it this way over having one Gigantic File that is unreadable
# Should allow to Generate multible builds at once

message(STATUS "Checking origin OS:")
if(UNIX)
    message(STATUS "Creating Project from Linux")
    if(ToAndroid)   
        include(${FlyLib_Modules}/FromLinux/LinuxToAndroid.cmake)
    elseif(ToWindows)
        include(${FlyLib_Modules}/FromLinux/LinuxToWindows.cmake)
    elseif(ToLinux)
        include(${FlyLib_Modules}/FromLinux/LinuxToLinux.cmake)
    endif()
elseif(WIN32)
    message(STATUS "Creating Project from Windows")
    if(ToAndroid)
        include(${FlyLib_Modules}/FromWin/WinToAndroid.cmake)
    elseif(ToLinux)
        include(${FlyLib_Modules}/FromWin/WinToLinux.cmake)
    elseif(ToWindows)
        include(${FlyLib_Modules}/FromWin/WinToWin.cmake)
    endif()
endif()

set_property(TARGET FlyLib PROPERTY POSITION_INDEPENDENT_CODE ON)

set_target_properties(FlyLib PROPERTIES
    PUBLIC_HEADER "fly_lib.h"
)