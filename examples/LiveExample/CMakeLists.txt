cmake_minimum_required(VERSION 3.1)

# Add path to where base CMakeLists.txt is for SqueLib
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ ${CMAKE_CURRENT_BINARY_DIR}/SqueLib)

project(LiveExample DESCRIPTION "Live Example for a Video"
                    LANGUAGES C CXX)

SqueLib_PrepareBuild(LiveExample MarcTorresJimenez ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp "")
SqueLib_Package("NoAssets" "${SqueLib_defaults}/Resources")
