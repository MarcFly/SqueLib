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
set(SQUE_cmake  ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)
set(SQUE_cmake_par ${SQUE_cmake} PARENT_SCOPE)
#include(${SQUE_cmake}/Helpers.cmake)
#------------------------------------------------------------------------------------------------
# SETUP CURRENT PROJECT
#------------------------------------------------------------------------------------------------
project(SqueLib     VERSION 2021.1
                    DESCRIPTION "C++ Library for getting introduced to multiplatform C++ development."
                    LANGUAGES C CXX)

# C/CXX Requirements
set(CMAKE_CXX_STANDARD 17) # Currently using <filesystem> for easy output and folder creation
# Probably I can downgrade to C++11 because most new things I don't really use with thought into it
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON) # Android Turns it off (i still don't know why but it should)

# Include directories shared by all build types
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/squelib_src)

# Get all shared source files
file(GLOB_RECURSE SQUELIB_SRC "${CMAKE_CURRENT_SOURCE_DIR}/squelib_src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/squelib_src/*.h")
add_library(SqueLib SHARED "${SQUELIB_SRC}")
#------------------------------------------------------------------------------------------------
# SETUP BUILD PATHS
#------------------------------------------------------------------------------------------------
execute_process(COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/builds)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/builds)
if(ToAndroid)
    set(SQUE_OutputFolder ${CMAKE_BINARY_DIR}/makecapk/lib/arm64-v8a)
else()
    set(CMAKE_SYSTEM_NAME ${CMAKE_HOST_SYSTEM_NAME})
    # Change System Name to desired, 
    #then setup compiler and linkers and whatever is needed for crosscompiling
    set(SQUE_OutputFolder ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME})
endif()
message(STATUS SQUE_OutputFolder ${SQUE_OutputFolder})
execute_process(COMMAND mkdir -p "${SQUE_OutputFolder}")
#------------------------------------------------------------------------------------------------
# FIND SYSTEM LIBRARIES
#------------------------------------------------------------------------------------------------
if(ToLinux OR ToWindows)
    find_package(OpenGL REQUIRED) # Defines OPEN_gl_LIBRARY
    
    if(ToLinux)
        find_package(Threads REQUIRED) # Defines CMAKE_THREAD_LIBS_INIT
        find_package(X11 REQUIRED)  # Defines X11_LIBRARIES
    endif()

endif()
#------------------------------------------------------------------------------------------------
# ADD NON-FETCHABLE SUBPROJECTS
#------------------------------------------------------------------------------------------------
# GLAD---------------------------------------------------------------------------------------
add_library(glad STATIC "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/src/glad.c")
set(glad_include "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include")
target_include_directories(glad PUBLIC ${glad_include})
set_target_properties(glad
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${SQUE_OutputFolder}/archive"
        LIBRARY_OUTPUT_DIRECTORY "${SQUE_OutputFolder}"
        RUNTIME_OUTPUT_DIRECTORY "${SQUE_OutputFolder}"
)

# STB ----------------------------------------------------------------------------------------
set(stb_include "${CMAKE_CURRENT_SOURCE_DIR}/external/stb")
# PCG ----------------------------------------------------------------------------------------
set(pcg_include "${CMAKE_CURRENT_SOURCE_DIR}/external/pcg")
set(pcg_src ${CMAKE_CURRENT_SOURCE_DIR}/external/pcg/pcg_basic.c ${CMAKE_CURRENT_SOURCE_DIR}/external/pcg/pcg_basic.h)
# SOLOUD - MINIAUDIO BACKEND ONLY -------------------------------------------------------------------------------------

#------------------------------------------------------------------------------------------------
# FETCH SUBPROJECTS
#------------------------------------------------------------------------------------------------
include(FetchContent)

set(EXTERNAL ${CMAKE_CURRENT_BINARY_DIR}/external/SqueLib_deps)
set(FETCHCONTENT_BASE_DIR ${EXTERNAL})
message(STATUS "Pulling Dependencies to build along")

# DESKTOP DEPENDENCIES---------------------------------------------------------------------------
if(ToWindows OR ToLinux OR ToDesktop) 
    message(STATUS "Pulling DESKTOP Dependencies")
    # GLFW---------------------------------------------------------------------------------------
    option(BUILD_SHARED_LIBS "Build shared libraries" ON)
    option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
    option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
    option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
    option(GLFW_INSTALL "Generate installation target" OFF)
    option(GLFW_VULKAN_STATIC "Assume the Vulkan loader is linked with the application" OFF)
    FetchContent_Declare(glfw
        GIT_REPOSITORY "https://github.com/glfw/glfw.git"
        GIT_TAG 3.3.4
    )
    message(STATUS "Error not here")
    FetchContent_GetProperties(glfw) # Generates glfw_SOURCE_DIR and glfw_BINARY_DIR
    set(glfw_include ${glfw_SOURCE_DIR}/include)
    set(glfw_lib_path ${glfw_BINARY_DIR}/src/${CMAKE_BUILD_TYPE})
    # -------------------------------------------------------------------------------------------

    # MAKEAVAILABLE------------------------------------------------------------------------------
    message(STATUS "Error not here")
    FetchContent_MakeAvailable(glfw)
    message(STATUS "Error not here")
    set_target_properties(glfw
        PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${SQUE_OutputFolder}/archive"
            LIBRARY_OUTPUT_DIRECTORY "${SQUE_OutputFolder}"
            RUNTIME_OUTPUT_DIRECTORY "${SQUE_OutputFolder}"
    )
    message(STATUS "Error not here")
    # All MakeAvailable together, first we declare so that they can start populating i belive
    # MakeAvailable should check completion/availability and finish/start downloading
    # -------------------------------------------------------------------------------------------
endif()
#------------------------------------------------------------------------------------------------
# CUSTOM BUILD MODULES PER PLATFORM
#------------------------------------------------------------------------------------------------
set(CMAKE_SYSTEM_NAME ${CMAKE_HOST_SYSTEM_NAME})
if(ToAndroid)
    add_compile_definitions(ANDROID)

    set(app_glue "${NDK}/sources/android/native_app_glue/android_native_app_glue.c")
    target_sources(SqueLib PUBLIC ${app_glue}) # Add Android Files required 

    include(${SQUE_cmake}/SetupAndroidEnv.cmake)
    link_android_libc(SqueLib ${SQUE_OutputFolder})
    link_android_all_libs(SqueLib)
    include_android(SqueLib)

elseif(ToWindows OR ToLinux OR ToDesktop)
    #---------------------------------------------------------------------------------------------
    # SET MINIMAL C/CXX FLAGS
    #---------------------------------------------------------------------------------------------
    set(CMAKE_C_FLAGS "-v ")
    set(CMAKE_CXX_FLAGS "-v ")
    #---------------------------------------------------------------------------------------------
    # CROSS-COMPILING Windows-Linux
    #---------------------------------------------------------------------------------------------
    
    #---------------------------------------------------------------------------------------------
    # DESKTOP INCLUDES/LINKS
    #---------------------------------------------------------------------------------------------
    target_link_libraries(SqueLib PUBLIC
        ${OPEN_gl_LIBRARY}
        glfw
    )
    target_include_directories(SqueLib PUBLIC
        ${glfw_include}
    )
    #---------------------------------------------------------------------------------------------
    # PLATFORM SPECIFIC INCLUDE/LINKS
    #---------------------------------------------------------------------------------------------
    if(ToLinux)
        target_link_libraries(SqueLib PUBLIC
            ${CMAKE_THREAD_LIBS_INIT}
            ${X11_LIBRARIES}
            ${CMAKE_DL_LIBS}
        )
    elseif(ToWindows)
    endif()
endif()
#------------------------------------------------------------------------------------------------
# LINKING / ADDING INCLUDE AND SOURCE FILES FROM SUBPROJECTS
#------------------------------------------------------------------------------------------------
target_include_directories(SqueLib PUBLIC
    ${stb_include}
    ${pcg_include}
)
target_link_libraries(SqueLib PUBLIC
    glad
)
target_sources(SqueLib PRIVATE
    "${pcg_src}"
)

#------------------------------------------------------------------------------------------------
# SETTING FINAL PROPERTIES FOR THE PROJECT
#------------------------------------------------------------------------------------------------
target_compile_options(SqueLib BEFORE PUBLIC -fpermissive)
target_compile_definitions(SqueLib PUBLIC WITH_MINIAUDIO)

set_target_properties(SqueLib
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${SQUE_OutputFolder}/archive"
        LIBRARY_OUTPUT_DIRECTORY "${SQUE_OutputFolder}"
        RUNTIME_OUTPUT_DIRECTORY "${SQUE_OutputFolder}"
        POSITION_INDEPENDENT_CODE ON
        PUBLIC_HEADER "squelib.h"
)

target_compile_options(glad PUBLIC
    -fPIC
)

#------------------------------------------------------------------------------------------------
# Group Sources for IDEs
#------------------------------------------------------------------------------------------------
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/squelib_src"
    PREFIX "SqueLib"
    FILES ${SQUELIB_SRC})
source_group(/pcg FILES ${pcg_src})

set(SqueLib_include ${CMAKE_CURRENT_SOURCE_DIR}/squelib_src PARENT_SCOPE)
set(SqueLib_Output ${SQUE_OutputFolder} PARENT_SCOPE) 
set(SqueLib_defaults ${CMAKE_CURRENT_SOURCE_DIR}/defaults PARENT_SCOPE)
set(SqueLib_extra ${CMAKE_CURRENT_SOURCE_DIR}/extra PARENT_SCOPE)
set(SqueLib_external ${CMAKE_CURRENT_SOURCE_DIR}/external PARENT_SCOPE)

include(${SQUE_cmake}/SqueLib_Package.cmake PARENT_SCOPE)