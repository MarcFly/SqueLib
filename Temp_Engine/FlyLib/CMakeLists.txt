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
set(FL_cmake  ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)
include(${FL_cmake}/Helpers.cmake)
#------------------------------------------------------------------------------------------------
# SETUP CURRENT PROJECT
#------------------------------------------------------------------------------------------------
project(FlyLib_  VERSION 2020.1
                    DESCRIPTION "Multiplatform C++ Library for making apps and games easily"
                    LANGUAGES C CXX)

# C/CXX Requirements
set(CMAKE_CXX_STANDARD 17) # Currently using <filesystem> for easy output and folder creation
# Probably I can downgrade to C++11 because most new things I don't really use with thought into it
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON) # Android Turns it off (i still don't know why but it should)

# Include directories shared by all build types
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/flylib_src)

# Get all shared source files
file(GLOB_RECURSE FLYLIB_SRC "${CMAKE_CURRENT_SOURCE_DIR}/flylib_src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/flylib_src/*.h")
add_library(FlyLib SHARED "${FLYLIB_SRC}")
#------------------------------------------------------------------------------------------------
# SETUP BUILD PATHS
#------------------------------------------------------------------------------------------------
execute_process(COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/builds)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/builds)

if(ToAndroid)
    set(FL_OutputFolder ${CMAKE_BINARY_DIR}/makecapk/lib/arm64-v8a)
else()
    set(CMAKE_SYSTEM_NAME ${CMAKE_HOST_SYSTEM_NAME})
    # Change System Name to desired, 
    #then setup compiler and linkers and whatever is needed for crosscompiling
    set(FL_OutputFolder ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME})
endif()
execute_process(COMMAND mkdir -p "${FL_OutputFolder}")
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
#if(ToWindows OR ToLinux OR ToDarwin OR ToMacOS OR ToDesktop)
    # GLAD---------------------------------------------------------------------------------------
    add_library(glad STATIC "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/src/glad.c")
    set(glad_include "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include")
    target_include_directories(glad PUBLIC ${glad_include})
    set_target_properties(glad
        PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive"
            LIBRARY_OUTPUT_DIRECTORY "${FL_OutputFolder}"
            RUNTIME_OUTPUT_DIRECTORY "${FL_OutputFolder}"
    )
#endif()
#------------------------------------------------------------------------------------------------
# FETCH SUBPROJECTS
#------------------------------------------------------------------------------------------------
include(FetchContent)

set(EXTERNAL ${CMAKE_SOURCE_DIR}/external/FlyLib_deps)
set(FETCHCONTENT_BASE_DIR ${EXTERNAL})
message(STATUS "Pulling Dependencies to build along")
# MathGeoLib--------------------------------------------------------------------------------------
FetchContent_Declare(mathgeolib
    GIT_REPOSITORY https://github.com/juj/MathGeoLib.git
    GIT_TAG master
)
FetchContent_MakeAvailable(mathgeolib)
FetchContent_GetProperties(mathgeolib SOURCE_DIR mgl_src_dir)
set(mgl_src_dir ${mgl_src_dir}/src)
set_target_properties(MathGeoLib
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive"
        LIBRARY_OUTPUT_DIRECTORY "${FL_OutputFolder}"
        RUNTIME_OUTPUT_DIRECTORY "${FL_OutputFolder}"
)
# FlatBuffers------------------------------------------------------------------------------------
option(FLATBUFFERS_BUILD_TESTS OFF)
option(FLATBUFFERS_INSTALL OFF)
FetchContent_Declare(flatbuffers
    GIT_REPOSITORY https://github.com/google/flatbuffers.git
    GIT_TAG v1.12.0
)
FetchContent_MakeAvailable(flatbuffers)
FetchContent_GetProperties(flatbuffers SOURCE_DIR flatbuffers_src_dir)

set(flatbuffers_include "${flatbuffers_src_dir}/include/flatbuffers")
# If I want to use it locally instead of pulling the whole repo
#set(flatbuffers_include "${CMAKE_CURRENT_SOURCE_DIR}/external/flatbuffers-src/include/flatbuffers")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/external/flatbuffers-src"
#                 ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers-build
#                 EXCLUDE_FROM_ALL)

set_target_properties(flatbuffers
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive"
        LIBRARY_OUTPUT_DIRECTORY "${FL_OutputFolder}"
        RUNTIME_OUTPUT_DIRECTORY "${FL_OutputFolder}"
)

# DESKTOP DEPENDENCIES---------------------------------------------------------------------------
if(ToWindows OR ToLinux OR ToDarwin OR ToMacOS OR ToDesktop) 
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
        GIT_TAG "1adfbde4d7fb862bb36d4a20e05d16bf712170f3"
    )
    FetchContent_GetProperties(glfw) # Generates glfw_SOURCE_DIR and glfw_BINARY_DIR
    set(glfw_include ${glfw_SOURCE_DIR}/include)
    set(glfw_lib_path ${glfw_BINARY_DIR}/src/${CMAKE_BUILD_TYPE})
    # -------------------------------------------------------------------------------------------

    # MAKEAVAILABLE------------------------------------------------------------------------------
    FetchContent_MakeAvailable(glfw)
    set_target_properties(glfw
        PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive"
            LIBRARY_OUTPUT_DIRECTORY "${FL_OutputFolder}"
            RUNTIME_OUTPUT_DIRECTORY "${FL_OutputFolder}"
    )
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
    include(${FL_cmake}/FindAndroidSDKVars.cmake)

    set(app_glue "${NDK}/sources/android/native_app_glue/android_native_app_glue.c")
    target_sources(FlyLib PUBLIC ${app_glue}) # Add Android Files required 

    include(${FL_cmake}/SetupAndroidEnv.cmake)
    set_android_link_flags()
    set_android_compile_flags()
    if(NOT ANDROIDVERSION)
        set(ANDROIDVERSION 29)
    endif()
    set_android_compiler(${NDK} ${OS_NAME} ${ANDROIDVERSION})
    link_android_all_libs(FlyLib ${NDK} ${OS_NAME} ${ANDROIDVERSION})
    include_android(FlyLib ${NDK} ${OS_NAME})

elseif(ToWindows OR ToLinux OR ToDarwin OR ToMacOS OR ToDesktop)
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
    target_link_libraries(FlyLib PUBLIC
        ${OPEN_gl_LIBRARY}
        glfw
    )
    target_include_directories(FlyLib PUBLIC
        ${glfw_include}
    )
    #---------------------------------------------------------------------------------------------
    # PLATFORM SPECIFIC INCLUDE/LINKS
    #---------------------------------------------------------------------------------------------
    if(ToLinux)
        target_link_libraries(FlyLib PUBLIC
            ${CMAKE_THREAD_LIBS_INIT}
            ${X11_LIBRARIES}
            ${CMAKE_DL_LIBS}
        )
    elseif(ToWindows)
    elseif(ToDarawin OR ToMacOS)
        target_link_libraries(FlyLib PUBLIC
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
        )
    endif()
endif()
#------------------------------------------------------------------------------------------------
# LINKING / ADDING INCLUDE AND SOURCE FILES FROM SUBPROJECTS
#------------------------------------------------------------------------------------------------
target_include_directories(FlyLib PUBLIC
    ${mgl_src_dir}
    ${flatbuffers_include}
)
target_link_libraries(FlyLib PUBLIC
    glad
    MathGeoLib
    flatbuffers
)
#------------------------------------------------------------------------------------------------
# SETTING FINAL PROPERTIES FOR THE PROJECT
#------------------------------------------------------------------------------------------------
target_compile_options(FlyLib BEFORE PUBLIC -fpermissive -funsafe-math-optimizations)
set_target_properties(FlyLib
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive"
        LIBRARY_OUTPUT_DIRECTORY "${FL_OutputFolder}"
        RUNTIME_OUTPUT_DIRECTORY "${FL_OutputFolder}"
        POSITION_INDEPENDENT_CODE ON
        PUBLIC_HEADER "fly_lib.h"
)
target_compile_options(glad PUBLIC
    -fPIC
)

#------------------------------------------------------------------------------------------------
# Group Sources for IDEs
#------------------------------------------------------------------------------------------------
#source_group("external/mathgeolib/source" FILES "${mathgeolib_src}")
source_group("flylib_source" FILES "${FLYLIB_SRC}")