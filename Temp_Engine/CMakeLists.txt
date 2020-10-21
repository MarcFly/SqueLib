cmake_minimum_required(VERSION 3.1...3.18) #require a minimum cmake version to build

# If the version is less than tested, make a warning about it, else try to use 3.18
if(${CMAKE_VERSION} VERSION_LESS 3.18)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.18)
endif()

#Main Global Variables
set(PROJ_NAME Temp_Engine)

#Setup Project
project(${PROJ_NAME}    VERSION 2020.1
                        DESCRIPTION "Multiplatform C++ Engine"
                        LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

#UNIX Things
if(UNIX)
#C++ STD Library Linkings
link_libraries(stdc++fs pthread)
endif(UNIX)

#Adding subfile Directories
set(HELPER_DIR "./EngineCode/helpers")
set(APP_DIR "./EngineCode/application")
set(MODULE_DIR "./EngineCode/modules")

#Include the directories for code to use
include_directories(${CMAKE_SOURCE_DIR})
include_directories(HELPER_DIR)
include_directories(APP_DIR)
include_directories(MODULE_DIR)

#Instead of hard coding the filenames, traverse the directory for the files
file(GLOB_RECURSE HELPERS "${HELPER_DIR}/*.cpp" "${HELPER_DIR}/*.h")
file(GLOB_RECURSE APP_SOURCE "${APP_DIR}/*.cpp" "${APP_DIR}/*.h")
file(GLOB_RECURSE MODULES "${MODULE_DIR}/*.cpp" "${MODULE_DIR}/*.h")

#Set Features for compilation of libraries
#target_compile_features(SDL2 PUBLIC cxx_std_11)
#target_compile_features(ImGui PUBLIC cxx_std_11)

#Add an executable entry point
add_executable(${PROJ_NAME} "./EngineCode/main.cpp")

#Target Sources for Own Engine things
target_sources(${PROJ_NAME} PUBLIC ${HELPERS} ${MODULES} ${APP_SOURCE})

#Make Folders inside IDEs
source_group("helpers" FILES ${HELPERS})
source_group("application" FILES ${APP_SOURCE})
source_group("modules" FILES ${MODULES})

#Link Libraries to target executable