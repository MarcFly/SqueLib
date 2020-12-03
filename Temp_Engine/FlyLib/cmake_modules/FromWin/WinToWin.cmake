#------------------------------------------------------------------------------------------------
# SET MINIMAL C/CXX FLAGS
#------------------------------------------------------------------------------------------------
set(CMAKE_C_FLAGS "-v -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "-v -fvisibility=hidden")
#------------------------------------------------------------------------------------------------
# SETUP PROJECT BINARY OUTPUT
#------------------------------------------------------------------------------------------------
add_library(FlyLib SHARED "${FLYLIB_SOURCE}")
#------------------------------------------------------------------------------------------------
# SETTING PROJECT OUTPUT PATHS
#------------------------------------------------------------------------------------------------
set(FL_OutputFolder "${CMAKE_BINARY_DIR}/${CMAKE_HOST_SYSTEM_NAME}")
execute_process(COMMAND mkdir -p ${FL_OutputFolder})

set_target_properties(FlyLib
    PROPERTIES 
        ARCHIVE_OUTPUT_DIRECTORY "${FL_OutputFolder}"
        LIBRARY_OUTPUT_DIRECTORY "${FL_OutputFolder}"
        RUNTIME_OUTPUT_DIRECTORY "${FL_OutputFolder}"
)