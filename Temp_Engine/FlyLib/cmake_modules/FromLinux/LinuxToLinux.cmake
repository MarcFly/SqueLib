set(CMAKE_C_FLAGS "-v ")
set(CMAKE_CXX_FLAGS "-v ")

set(LTL_FLYLIB "${PROJECT_NAME}_LTL") # Specifying name for avoiding name clashing when multiple builds 
add_library(${LTL_FLYLIB} STATIC 
    ${FLYLIB_SOURCE}) # Lib Sources

set(LLinuxx64 "${CMAKE_BINARY_DIR}/lib/linux_x64")
set(BLinuxx64 "${CMAKE_BINARY_DIR}/bin/linux_x64")
execute_process(COMMAND mkdir -p ${LLinuxx64})
execute_process(COMMAND mkdir -p ${BLinuxx64})

set_target_properties(${LTL_FLYLIB}
    PROPERTIES 
        ARCHIVE_OUTPUT_DIRECTORY "${LLinuxx64}"
        LIBRARY_OUTPUT_DIRECTORY "${LLinuxx64}"
        RUNTIME_OUTPUT_DIRECTORY "${BLinuxx64}"
)

# Set Required includes from Android NDK
#target_include_directories(${LTA_FLYLIB} PUBLIC )

# Include External libraries (header only, source code,...) to own project if needed

# Call build to external libraries

# Link to the built/prebuilt external libraries

# Link to Android Prebuilt Libraries
target_link_libraries(${LTL_FLYLIB} PUBLIC 
    stdc++fs 
    pthread
)

#Link Libraries to target executable