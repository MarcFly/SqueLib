cmake_minimum_required(VERSION 3.1...3.18) #require a minimum cmake version to build

# If the version is less than tested, make a warning about it, else try to use 3.18
if(${CMAKE_VERSION} VERSION_LESS 3.18)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.18)
endif()

#Setup Project
project(Temp_Engine VERSION 2020.1
                    DESCRIPTION "Multiplatform C++ Engine"
                    LANGUAGES CXX)

#Base Include Directory
#target_include_directories(Temp_Engine PUBLIC "./EngineCode")

#Adding subfile Directories
set(HELPER_DIR "./EngineCode/helpers")
include_directories(${CMAKE_SOURCE_DIR})
include_directories(HELPER_DIR)

set(HELPERS "${HELPER_DIR}/LogHandler.cpp" "${HELPER_DIR}/LWTimer.cpp" "${HELPER_DIR}/SimpleProfiler.cpp")

#Set Features for compilation of libraries
#target_compile_features(SDL2 PUBLIC cxx_std_11)
#target_compile_features(ImGui PUBLIC cxx_std_11)

#Finally Add an executable entry point
add_executable(Temp_Engine "./EngineCode/main.cpp")
target_sources(Temp_Engine PUBLIC ${HELPERS})


#Link Libraries to target executable