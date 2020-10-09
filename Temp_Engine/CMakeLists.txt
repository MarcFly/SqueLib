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

#Adding subfile Directories
set(HELPER_DIR "./EngineCode/helpers")

#Include the directories for code to use
include_directories(${CMAKE_SOURCE_DIR})
include_directories(HELPER_DIR)

#set(HELPERS "${HELPER_DIR}/LogHandler.cpp" "${HELPER_DIR}/LWTimer.cpp" "${HELPER_DIR}/SimpleProfiler.cpp")

#Instead of hard coding the filenames, traverse the directory for the files
file(GLOB_RECURSE HELPERS "${HELPER_DIR}/*.cpp" "${HELPER_DIR}/*.h")

#Set Features for compilation of libraries
#target_compile_features(SDL2 PUBLIC cxx_std_11)
#target_compile_features(ImGui PUBLIC cxx_std_11)

#Add an executable entry point
add_executable(${PROJ_NAME} "./EngineCode/main.cpp")
target_sources(${PROJ_NAME} PUBLIC ${HELPERS})

source_group("helpers" FILES ${HELPERS})

#Link Libraries to target executable