add_compile_definitions(ANDROID)

#=========================================================================

# Set Route to find SDKs
set(OS_NAME linux-x86_64)

if(Windows)
    set(OS_NAME windows-x86_64)
endif(Windows)

if(Darwin)
    set(OS_NAME darwin-x86-64)
endif(Darwin)

message(STATUS "Building from " ${OS_NAME})

# Check that Environment Variable for SDK Exists, else define it
if(NOT ANDROIDSDK)
    set(SDK_DIRS $ENV{SDK_LOCATIONS} $ENV{ANDROID_HOME} $ENV{ANDROID_SDK_ROOT} ~/Android/Sdk $ENV{HOME}/Android/Sdk)
    first_exists(ANDROIDSDK "${SDK_DIRS}")
endif()

# Search for NDK inside SDK
if(NDK)
    message(STATUS "NDK already defined at ${NDK}")
    set(NDK $ENV{NDK})
elseif($ENV{ANDROID_NDK})
    message(STATUS "NDK already defined at $ENV{ANDROID_NDK}")
    set(NDK $ENV{ANDROID_NDK})
elseif($ENV{ANDROID_NDK_HOME})
    message(STATUS "NDK already defined at $ENV{ANDROID_NDK_HOME}")
    set(NDK $ENV{ANDROID_NDK_HOME})
else()
    set(NDK_DIRS ${ANDROIDSDK}/ndk ${ANDROIDSDK}/ndk-bundle)
    first_exists(NDK_G "${NDK_DIRS}")
    subdirlist(NDK_L ${NDK_G})
    first_exists(NDK "${NDK_L}")
endif()

# Search for Build Tools inside SDK
first_exists(BT_EXISTS ${ANDROIDSDK}/build-tools)
if(NOT EXISTS ${BT_EXISTS})
    message(FATAL_ERROR "Build tools folder not found")
endif()

subdirlist(BT_Vrs ${BT_EXISTS})
first_exists(BUILD_TOOLS "${BT_Vrs}")
set(AAPT ${BUILD_TOOLS}/aapt)

subdirlist(BT_Vrs ${BT_EXISTS})
first_exists(BUILD_TOOLS "${BT_Vrs}")

# Check that everything was found
if(NOT EXISTS ${ANDROIDSDK})
    message(FATAL_ERROR "ANDROID SDK NOT FOUND")
endif()
if(NOT EXISTS ${NDK})
    message(FATAL_ERROR "ANDROID NDK NOT FOUND")
endif()
if(NOT EXISTS ${BUILD_TOOLS})
    message(FATAL_ERROR "ANDROID BUILD_TOOLS NOT FOUND")
endif()

message(STATUS ${ANDROIDSDK})
message(STATUS ${NDK})
message(STATUS ${BUILD_TOOLS})
message(STATUS)

#=========================================================================

set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_VERSION 29)
set(CMAKE_ANDROID_ARCH_AI arm64-v8a)
set(CMAKE_ANDROID_NDK ${NDK})
set(CMAKE_ANDROID_STL_TYPE c++_shared)

set(${ANDROID_NDK} ${NDK})

set(ANDROID_ABI arm64-v8a)
set(ANDROID_LD lld)
set(ANDROID_PLATFORM 24)
set(ANDROID_STL c++_shared)

# Getting code and setting properties to compile SharedLib

set(ANDROID_DIR ${CMAKE_SOURCE_DIR}/EngineCode/android)
set(app_glue "${NDK}/sources/android/native_app_glue/android_native_app_glue.c")

# NDK App Glue added directly to project to compile
add_library(${PROJ_NAME} SHARED 
    ./EngineCode/android/AndroidGlue.cpp # Entry Point and code to interact with android 
    ${app_glue}  # NDK Native App Glue
    ${ENGINE_SOURCES}) # Engine Code


# Link to necessary NDK Directories

set(NDKLIBV ${NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/aarch64-linux-android/${CMAKE_SYSTEM_VERSION} )
set(NDKINCLUDE ${NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include)
set(CMAKE_CXX_FLAGS " -L${NDKLIBV} -B${NDKLIBV} -L/usr/lib/gcc/x86_64-linux-gnu/9 ${CMAKE_CXX_FLAGS} -v ")

target_include_directories(${PROJ_NAME} PUBLIC    
    ${NDK}/sources/android/native_app_glue
    ${NDK}/sources/cxx-stl/llvm-libc++/include
    ${NDKINCLUDE}
    ${NDKINCLUDE}/android
    ${NDKINCLUDE}/aarch64-linux-android
)

include(${NDK}/build/cmake/android.toolchain.cmake)