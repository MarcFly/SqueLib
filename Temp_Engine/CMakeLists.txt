cmake_minimum_required(VERSION 3.1...3.18) #require a minimum cmake version to build

# Helper Modules
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/FlyLib)
include(Helpers)

# If the version is less than tested, make a warning about it, else try to use 3.18
if(${CMAKE_VERSION} VERSION_LESS 3.18)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.18)
endif()


#Setup Project
project(Temp_Engine     VERSION 2020.1
                        DESCRIPTION "Multiplatform C++ Engine"
                        LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Things that are shared between all builds
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/EngineCode)
execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/builds)
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/builds)

# The Engine will be provided as is, no options initally to select
# Maybe in future yes for installing submodules of the engine
file(GLOB_RECURSE ENGINE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/EngineCode/main.cpp")

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
        include(FromWin/WinToAndroid)
    elseif(ToLinux)
        include(FromWin/WinToLinux)
    elseif(ToWindows)
        include(FromWin/WinToWin)
    endif()
endif()

# Links and Includes
target_link_libraries(TempEngine PUBLIC
    FlyLib
)

target_include_directories(TempEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/FlyLib/flylib_src
)

