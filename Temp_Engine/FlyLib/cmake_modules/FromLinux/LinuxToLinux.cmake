set(CMAKE_C_FLAGS "-v -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "-v -fvisibility=hidden")

add_library(FlyLib SHARED 
    "${FLYLIB_SOURCE}") # Lib Sources

set(BLinux "${CMAKE_BINARY_DIR}/LinuxX64")
execute_process(COMMAND mkdir -p ${BLinux})

set_target_properties(FlyLib
    PROPERTIES 
        ARCHIVE_OUTPUT_DIRECTORY "${BLinux}"
        LIBRARY_OUTPUT_DIRECTORY "${BLinux}"
        RUNTIME_OUTPUT_DIRECTORY "${BLinux}"
)

# Set Required includes from Android NDK
#target_include_directories(${LTA_FLYLIB} PUBLIC )

# Include External libraries (header only, source code,...) to own project if needed

# Call build to external libraries

# Link to the built/prebuilt external libraries

# Link to Android Prebuilt Libraries
target_link_libraries(FlyLib PUBLIC 
    stdc++fs 
    pthread
)

#Link Libraries to target executable