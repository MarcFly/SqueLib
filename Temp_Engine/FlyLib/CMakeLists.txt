# Set bae CMake values
cmake_minimum_required(VERSION 3.1...3.18) #require a minimum cmake version to build
# If the version is less than tested, make a warning about it, else try to use 3.18
if(${CMAKE_VERSION} VERSION_LESS 3.18)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.18)
endif()

project(FlyLib)

set(. ${CMAKE_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${.}/cmake_modules)

# Set Naming Vars
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON) # For Android have to be turned off

# Include External Modules for helper functions
include(External/Helpers)

# Things that are shared between all builds
include_directories(${CMAKE_SOURCE_DIR}/flylib_src)
execute_process(COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/builds)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/builds)

# Do a big ifdef for specific parts of the library
# First if if full library define
# Ifdefs for not using parts, for now full build

file(GLOB_RECURSE FLYLIB_SOURCE "${.}/flylib_src/*.cpp")

# Include modules for creating builds to diferent platforms
# Requires file per platform to platform
# I prefer it this way over having one Gigantic File that is unreadable
# Should allow to Generate multible builds at once

if(UNIX)
    if(ToAndroid)
        include(FromLinux/LinuxToAndroid)
    elseif(ToWindows)
        include(FromLinux/LinuxToWindows)
    elseif(ToLinux)
        include(FromLinux/LinuxToLinux)
    endif()
elseif(WINDOWS)
    if(ToAndroid)
        include(WinToAndroid)
    endif()
    if(ToLinux)
        include(WinToLinux)
    endif()
    include(WinToWin)
endif()