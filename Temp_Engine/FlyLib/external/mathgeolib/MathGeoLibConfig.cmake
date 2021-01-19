# - Config file for the MathGeoLib package

# Compute paths
get_filename_component(MATHGEOLIB_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(MATHGEOLIB_INCLUDE_DIRS "C:/Users/MarcFly/Documents/Repos/TFG-TempEngine/Temp_Engine/external/FlyLib_deps/mathgeolib-src;C:/Users/MarcFly/Documents/Repos/TFG-TempEngine/Temp_Engine/external/FlyLib_deps/mathgeolib-src")

# Our library dependencies (contains definitions for IMPORTED targets)
if(NOT TARGET MathGeoLib)
  include("${MATHGEOLIB_CMAKE_DIR}/MathGeoLibTargets.cmake")
endif()

# These are IMPORTED targets created by MathGeoLibTargets.cmake
set(MATHGEOLIB_LIBRARIES MathGeoLib)
