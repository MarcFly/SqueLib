cmake_minimum_required(VERSION 3.1...3.18) #require a minimum cmake version to build
# If the version is less than tested, make a warning about it, else try to use 3.18
if(${CMAKE_VERSION} VERSION_LESS 3.18)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.18)
endif()
#------------------------------------------------------------------------------------------------
# ADD SUBPROJECTS # Add where SqueLib is
#------------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ ${CMAKE_CURRENT_BINARY_DIR}/SqueLib)
#------------------------------------------------------------------------------------------------
# SETUP CURRENT PROJECT
#------------------------------------------------------------------------------------------------
project(SquePerf        VERSION 2020.1
                        DESCRIPTION "Multiplatform C++ Engine"
                        LANGUAGES C CXX)
# C/CXX Requirements
set(CMAKE_CXX_STANDARD 17) # Currently using <filesystem> for easy output and folder creation
# Probably I can downgrade to C++11 because most new things I don't really use with thought into it
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON) # Android Turns it off (i still don't know why but it should)

#------------------------------------------------------------------------------------------------
# SETUP BUILD PATHS
#------------------------------------------------------------------------------------------------
execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/builds)
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/builds)
#------------------------------------------------------------------------------------------------
# REQUIRED PLATFORM SPECIFICS
#------------------------------------------------------------------------------------------------

SqueLib_PrepareBuild(SquePerf MarcTorresJimenez "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
SqueLib_Package("" "")

#------------------------------------------------------------------------------------------------
# SETTING FINAL PROPERTIES FOR THE PROJECT
#------------------------------------------------------------------------------------------------
target_compile_options(SqueLib BEFORE PUBLIC -fpermissive)
set_target_properties(SquePerf
    PROPERTIES 
        ARCHIVE_OUTPUT_DIRECTORY "${SQUE_OutputFolder}/archive"
        LIBRARY_OUTPUT_DIRECTORY "${SqueLib_Output}"
        RUNTIME_OUTPUT_DIRECTORY "${SqueLib_Output}"
)
# INCLUDE AND LINK TO SQUELIB
target_link_libraries(SquePerf 
    PUBLIC 
        SqueLib)
message(STATUS ${SqueLib_Output})
target_include_directories(SquePerf 
    PUBLIC 
        ${SqueLib_include})