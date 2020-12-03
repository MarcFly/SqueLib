#------------------------------------------------------------------------------------------------
# SET MINIMAL C/CXX FLAGS
#------------------------------------------------------------------------------------------------
set(CMAKE_C_FLAGS "-v ")
set(CMAKE_CXX_FLAGS "-v ")
#------------------------------------------------------------------------------------------------
# SETUP PROJECT OUTPUT
#------------------------------------------------------------------------------------------------
add_executable(TempEngine "${ENGINE_SRC}")

set(TE_OutputFolder "${CMAKE_BINARY_DIR}/${CMAKE_HOST_SYSTEM_NAME}")
set_target_properties(TempEngine
    PROPERTIES 
        ARCHIVE_OUTPUT_DIRECTORY "${TE_OutputFolder}"
        LIBRARY_OUTPUT_DIRECTORY "${TE_OutputFolder}"
        RUNTIME_OUTPUT_DIRECTORY "${TE_OutputFolder}"
)
#------------------------------------------------------------------------------------------------
# PLATFORM SPECIFIC INCLUDE/LINKS
#------------------------------------------------------------------------------------------------