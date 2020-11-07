cmake_minimum_required(VERSION 3.1...3.18) #require a minimum cmake version to build

set(. ${CMAKE_SOURCE_DIR})

# Helper Modules
set(CMAKE_MODULE_PATH ${.}/cmake_modules)
include(Helpers)


# If the version is less than tested, make a warning about it, else try to use 3.18
if(${CMAKE_VERSION} VERSION_LESS 3.18)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.18)
endif()

#Main Global Variables
set(PROJ_NAME "TempEngine")

#Setup Project
project(${PROJ_NAME}    VERSION 2020.1
                        DESCRIPTION "Multiplatform C++ Engine"
                        LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

#Adding subfile Directories
set(HELPER_DIR "${.}/EngineCode/helpers")
set(APP_DIR "${.}/EngineCode/application")
set(MODULE_DIR "${.}/EngineCode/modules")

#Instead of hard coding the filenames, traverse the directory for the files
set(MAIN "${.}/EngineCode/main.cpp")
file(GLOB_RECURSE HELPERS "${HELPER_DIR}/*.cpp")
file(GLOB_RECURSE APP_SOURCE "${APP_DIR}/*.cpp")
file(GLOB_RECURSE MODULES "${MODULE_DIR}/*.cpp")

#ENGINE SOURCES, for usage in other compile directives easily
set(ENGINE_SOURCES ${MAIN} "${HELPERS}" "${APP_SOURCE}" "${MODULES}")

#Include the directories for code to use
include_directories(${CMAKE_SOURCE_DIR})
include_directories(HELPER_DIR)
include_directories(APP_DIR)
include_directories(MODULE_DIR)

#UNIX Things
if(UNIX)
    if(NOT ANDROID)
        include(Linux)
    else()
        include(ACmake)
    endif()

elseif(WINDOWS)
    include(Windows)
endif()

