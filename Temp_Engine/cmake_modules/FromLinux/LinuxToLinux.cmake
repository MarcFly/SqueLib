set(CMAKE_C_FLAGS "-v ")
set(CMAKE_CXX_FLAGS "-v ")

add_executable(TempEngine "${ENGINE_SRC}")

set(BLinux "${CMAKE_BINARY_DIR}/LinuxX64")

set_target_properties(TempEngine
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
set(CMAKE_CXX_FLAGS "-L${BLinux}/libFlyLib.so ${CMAKE_CXX_FLAGS} -lFlyLib")

target_link_directories(TempEngine PUBLIC
    ${BLinux}
)

#Link Libraries to target executable