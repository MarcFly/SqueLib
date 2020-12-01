set(CMAKE_C_FLAGS "-v -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "-v -fvisibility=hidden")

add_library(FlyLib SHARED 
    "${FLYLIB_SOURCE}") # Lib Sources

set(BWin "${CMAKE_BINARY_DIR}/Winx64")
execute_process(COMMAND mkdir -p ${BLinux})

set_target_properties(FlyLib
    PROPERTIES 
        ARCHIVE_OUTPUT_DIRECTORY "${BWin}"
        LIBRARY_OUTPUT_DIRECTORY "${BWin}"
        RUNTIME_OUTPUT_DIRECTORY "${BWin}"
)

# Set Required includes from Android NDK
#target_include_directories(${LTA_FLYLIB} PUBLIC )

# Include External libraries (header only, source code,...) to own project if needed

# Call build to external libraries

# Link to the built/prebuilt external libraries

#Link Libraries to target executable