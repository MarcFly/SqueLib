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

project(SqueArkanoid    VERSION 2020.1
                    DESCRIPTION "Arkanoid Example for Squelib"
                    LANGUAGES C CXX)
# C/CXX Requirements
set(CMAKE_CXX_STANDARD 17) # Currently using <filesystem> for easy output and folder creation
# Probably I can downgrade to C++11 because most new things I don't really use with thought into it
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON) # Android Turns it off (i still don't know why but it should)

#------------------------------------------------------------------------------------------------
# SETUP WITH SQUELIB - Generates a target and sets it up to work with SqueLib
#------------------------------------------------------------------------------------------------

file(GLOB_RECURSE ARK_SRC "${CMAKE_CURRENT_SOURCE_DIR}/code/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/code/*.h")


SqueLib_PrepareBuild(SqueArkanoid MarcTorresJimenez "${ARK_SRC}" "${CMAKE_CURRENT_SOURCE_DIR}/code")
SqueLib_Package("${CMAKE_CURRENT_SOURCE_DIR}/Assets" "${CMAKE_CURRENT_SOURCE_DIR}/Resources")

#------------------------------------------------------------------------------------------------
# CONTINUE SETUP
#------------------------------------------------------------------------------------------------

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/code" 
    PREFIX "code" 
    FILES ${ARK_SRC}
)