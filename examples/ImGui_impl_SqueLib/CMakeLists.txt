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
# Binary dir will be changed to the expected binary dir to have together shared libraries
#------------------------------------------------------------------------------------------------
# FETCH SUBPROJECTS # Downloads dependencies that can be setup easily but a pain to do manually
#------------------------------------------------------------------------------------------------
include(FetchContent)
set(EXTERNAL ${CMAKE_CURRENT_BINARY_DIR}/external/ImGui_Example_deps)
set(FETCHCONTENT_BASE_DIR ${EXTERNAL})
execute_process(COMMAND mkdir -p ${EXTERNAL})
# DearIMGUI--------------------------------------------------------------------------------------
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG "v1.79"
)
FetchContent_GetProperties(imgui imgui_SOURCE_DIR imgui_include) # Generates imgui_SOURCE_DIR and imgui_BINARY_DIR
FetchContent_MakeAvailable(imgui)   
set(imgui_include ${imgui_SOURCE_DIR})
set(imgui_src
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_include}/imgui_draw.cpp
    ${imgui_include}/imgui_widgets.cpp
    ${imgui_include}/imgui.cpp
)
set(imgui_squelib ${SqueLib_extra}/imgui_squelib)
list(APPEND imgui_src ${imgui_squelib}/imgui_impl_squelib.cpp)
#------------------------------------------------------------------------------------------------
# SETUP CURRENT PROJECT
#------------------------------------------------------------------------------------------------
project(ImGui_Example     VERSION 2020.1
                        DESCRIPTION "Multiplatform C++ Engine"
                        LANGUAGES C CXX)
# C/CXX Requirements
set(CMAKE_CXX_STANDARD 17) # Currently using <filesystem> for easy output and folder creation
# Probably I can downgrade to C++11 because most new things I don't really use with thought into it
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON) # Android Turns it off (i still don't know why but it should)

# The Engine will be provided as is, no options initally to select
# Maybe in future yes for installing submodules of the engine
file(GLOB_RECURSE ALL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp")
#------------------------------------------------------------------------------------------------
# SETUP BUILD PATHS
#------------------------------------------------------------------------------------------------
execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/builds)
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/builds)
#------------------------------------------------------------------------------------------------
# REQUIRED PLATFORM SPECIFICS
#------------------------------------------------------------------------------------------------

SqueLib_PrepareBuild(ImGui_Example MarcTorresJimenez ${ALL_SRC})
SqueLib_Package(${CMAKE_CURRENT_SOURCE_DIR}/EngineAssets ${CMAKE_CURRENT_SOURCE_DIR}/EngineResources)

#------------------------------------------------------------------------------------------------
# SETTING FINAL PROPERTIES FOR THE PROJECT
#------------------------------------------------------------------------------------------------
target_compile_options(SqueLib BEFORE PUBLIC -fpermissive)
set_target_properties(ImGui_Example
    PROPERTIES 
        ARCHIVE_OUTPUT_DIRECTORY "${SQUE_OutputFolder}/archive"
        LIBRARY_OUTPUT_DIRECTORY "${SqueLib_Output}"
        RUNTIME_OUTPUT_DIRECTORY "${SqueLib_Output}"
)
# INCLUDE AND LINK TO SQUELIB
target_link_libraries(ImGui_Example 
    PUBLIC 
        SqueLib)
message(STATUS ${SqueLib_Output})
target_include_directories(ImGui_Example 
    PUBLIC 
        ${SqueLib_include}
        ${CMAKE_CURRENT_SOURCE_DIR}/source)

# INCLUDE AND LINK TO OTHER SUBPROJECTS
target_sources(ImGui_Example PUBLIC
    "${imgui_src}"
)
target_include_directories(ImGui_Example   PUBLIC
    ${imgui_include} ${imgui_squelib}
)