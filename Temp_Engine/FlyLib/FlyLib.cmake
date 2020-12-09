#------------------------------------------------------------------------------------------------
# HELPER MODULES
#------------------------------------------------------------------------------------------------
set(FL_cmake  ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)
include(${FL_cmake}/Helpers.cmake)

# C/CXX Requirements
set(CMAKE_CXX_STANDARD 17) # Currently using <filesystem> for easy output and folder creation
# Probably I can downgrade to C++11 because most new things I don't really use with thought into it
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON) # Android Turns it off (i still don't know why but it should)

# Include directories shared by all build types
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/FlyLib/flylib_src)

#------------------------------------------------------------------------------------------------
# ADD SUBPROJECTS
#------------------------------------------------------------------------------------------------
set(FL_EXTERNAL_DEPENDENCIES ${CMAKE_CURRENT_SOURCE_DIR}/FlyLib/dependencies)
execute_process(COMMAND mkdir -p ${FL_EXTERNAL_DEPENDENCIES})
include(FetchContent)
#------------------------------------------------------------------------------------------------
# CUSTOM BUILD MODULES PER PLATFORM
#------------------------------------------------------------------------------------------------
# Currently this is only needed for android
file(GLOB_RECURSE FlyLib_SRC ${CMAKE_CURRENT_SOURCE_DIR}/FlyLib/flylib_src/*.cpp)
set(APP_GLUE_SRC "${NDK}/sources/android/native_app_glue/android_native_app_glue.c")

add_library(FlyLib SHARED "${FlyLib_SRC}" ${APP_GLUE_SRC})
target_include_directories(FlyLib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/FlyLib/flylib_src)

set(FL_OutputFolder ${CMAKE_BINARY_DIR}/makecapk/lib/arm64-v8a)
set_target_properties(FlyLib
            PROPERTIES
                ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive"
                LIBRARY_OUTPUT_DIRECTORY "${FL_OutputFolder}"
                RUNTIME_OUTPUT_DIRECTORY "${FL_OutputFolder}")
set_android_link_flags()
set_android_compile_flags()

if(NOT ANDROIDVERSION)
    set(ANDROIDVERSION 29) # What you can test lowest
endif()
set_android_compiler(${NDK} ${OS_NAME} ${ANDROIDVERSION})
link_android_all_libs(FlyLib ${NDK} ${OS_NAME} ${ANDROIDVERSION})
include_android(FlyLib ${NDK})

#------------------------------------------------------------------------------------------------
# SETTING FINAL PROPERTIES FOR THE PROJECT
#------------------------------------------------------------------------------------------------
set_property(TARGET FlyLib PROPERTY POSITION_INDEPENDENT_CODE ON)

if(ToWindows OR ToLinux) # Link/Include desktop dependencies
    target_link_libraries(FlyLib PRIVATE
        glfw)
    target_include_directories(FlyLib PRIVATE
        ${glfw_include})
elseif(ToAndroid) # Link/Include Android Dependencies
    #target_link_libraries(FlyLib)
endif()