cmake_minimum_required(VERSION 3.1)

# Directory where SqueLib repo is
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ ${CMAKE_BINARY_DIR}/SqueLib)

project(LiveExampe DESCRIPTION "Live Example for Video"
                    LANGUAGES C CXX)

SqueLib_PrepareBuild(LiveExample MarcTorresJimenez "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" "NoIncludeDirs")
SqueLib_Package("NoAssets" "${SqueLib_defaults}/Resources")
