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
set(TE_cmake ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)
include(${TE_cmake}/Helpers.cmake)
#------------------------------------------------------------------------------------------------
# ADD SUBPROJECTS
#------------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/FlyLib)
#------------------------------------------------------------------------------------------------
# SETUP CURRENT PROJECT
#------------------------------------------------------------------------------------------------
project(Temp_Engine     VERSION 2020.1
                        DESCRIPTION "Multiplatform C++ Engine"
                        LANGUAGES CXX)
# C/CXX Requirements
set(CMAKE_CXX_STANDARD 17) # Currently using <filesystem> for easy output and folder creation
# Probably I can downgrade to C++11 because most new things I don't really use with thought into it
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON) # Android Turns it off (i still don't know why but it should)

# Include directories shared by all build types
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/EngineCode)

# The Engine will be provided as is, no options initally to select
# Maybe in future yes for installing submodules of the engine
file(GLOB_RECURSE ENGINE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/EngineCode/main.cpp")
#------------------------------------------------------------------------------------------------
# SETUP BUILD PATHS
#------------------------------------------------------------------------------------------------
execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/builds)
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/builds)
#------------------------------------------------------------------------------------------------
# CUSTOM BUILD MODULES PER PLATFORM
#------------------------------------------------------------------------------------------------
message(STATUS "Checking origin OS:")
if(UNIX)
    message(STATUS "Creating Project from Linux")
    if(ToAndroid)   
        include(${TE_cmake}/FromLinux/LinuxToAndroid.cmake)
    elseif(ToWindows)
        include(${TE_cmake}/FromLinux/LinuxToWindows.cmake)
    elseif(ToLinux)
        include(${TE_cmake}/FromLinux/LinuxToLinux.cmake)
    endif()
elseif(WIN32)
    message(STATUS "Creating Project from Windows")
    if(ToAndroid)
        include(${TE_cmake}/FromWin/WinToAndroid.cmake)
    elseif(ToLinux)
        include(${TE_cmake}/FromWin/WinToLinux.cmake)
    elseif(ToWindows)
        include(${TE_cmake}/FromWin/WinToWin.cmake)
    endif()
endif()

#------------------------------------------------------------------------------------------------
# SETTING FINAL PROPERTIES FOR THE PROJECT
#------------------------------------------------------------------------------------------------
# INCLUDE AND LINK TO FLYLIB
target_link_libraries(TempEngine PUBLIC FlyLib)
target_include_directories(TempEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/FlyLib/flylib_src)