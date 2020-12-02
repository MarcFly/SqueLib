cmake_minimum_required(VERSION 3.1...3.18) #require a minimum cmake version to build
# If the version is less than tested, make a warning about it, else try to use 3.18
if(${CMAKE_VERSION} VERSION_LESS 3.18)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.18)
endif()
#------------------------------------------------------------------------------------------------
# HELPER MODULES
#------------------------------------------------------------------------------------------------
# Instead of Using CMAKE_MODULE_PATH, generate a variable to go into it and use .cmake extension
# That way we don't overwrite other projects' CMAKE_MODULE_PATH and keep it cleaner to read
set(FL_cmake ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)
include(${FL_cmake}/External/Helpers.cmake)
#------------------------------------------------------------------------------------------------
# ADD SUBPROJECTS
#------------------------------------------------------------------------------------------------
set(FL_EXTERNAL_DEPENDENCIES ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)
execute_process(COMMAND mkdir -p ${FL_EXTERNAL_DEPENDENCIES})
include(FetchContent)

message(STATUS "Pulling Dependencies to build along")
if(ToWindows OR ToLinux) # Pull desktop dependencies
    # GLFW
    FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.2
        CMAKE_ARGS "-DBUILD_SHARED_LIBS=ON -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_DOCS=OFF")
    FetchContent_GetProperties(glfw)
    string(TOLOWER "glfw" lcName)
    if(NOT ${lcName}_POPULATED)
        FetchContent_Populate(${lcName})
        add_subdirectory(${${lcName}_SOURCE_DIR} ${${lcName}_BINARY_DIR})
    endif()
endif()

#------------------------------------------------------------------------------------------------
# SETUP CURRENT PROJECT
#------------------------------------------------------------------------------------------------
project(FlyLib  VERSION 2020.1
                    DESCRIPTION "Multiplatform C++ Library for making apps and games easily"
                    LANGUAGES CXX)

# C/CXX Requirements
set(CMAKE_CXX_STANDARD 17) # Currently using <filesystem> for easy output and folder creation
# Probably I can downgrade to C++11 because most new things I don't really use with thought into it
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON) # Android Turns it off (i still don't know why but it should)

# Include directories shared by all build types
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/flylib_src)

# Get all shared source files
file(GLOB_RECURSE FLYLIB_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/flylib_src/*.cpp")
#------------------------------------------------------------------------------------------------
# SETUP BUILD PATHS
#------------------------------------------------------------------------------------------------
execute_process(COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/builds)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/builds)
#------------------------------------------------------------------------------------------------
# CUSTOM BUILD MODULES PER PLATFORM
#------------------------------------------------------------------------------------------------
message(STATUS "Checking origin OS:")
if(UNIX)
    message(STATUS "Creating Project from Linux")
    if(ToAndroid)   
        include(${FL_cmake}/FromLinux/LinuxToAndroid.cmake)
    elseif(ToWindows)
        include(${FL_cmake}/FromLinux/LinuxToWindows.cmake)
    elseif(ToLinux)
        include(${FL_cmake}/FromLinux/LinuxToLinux.cmake)
    endif()
elseif(WIN32)
    message(STATUS "Creating Project from Windows")
    if(ToAndroid)
        include(${FL_cmake}/FromWin/WinToAndroid.cmake)
    elseif(ToLinux)
        include(${FL_cmake}/FromWin/WinToLinux.cmake)
    elseif(ToWindows)
        include(${FL_cmake}/FromWin/WinToWin.cmake)
    endif()
endif()
#------------------------------------------------------------------------------------------------
# SETTING FINAL PROPERTIES FOR THE PROJECT
#------------------------------------------------------------------------------------------------
set_property(TARGET FlyLib PROPERTY POSITION_INDEPENDENT_CODE ON)

set_target_properties(FlyLib PROPERTIES
    PUBLIC_HEADER "fly_lib.h"
)

if(ToWindows OR ToLinux) # Pull desktop dependencies
target_link_libraries(FlyLib glfw)
endif()