add_compile_definitions(ANDROID)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ANDROIDVERSION 29)

include(External/FindSDKVars) # Sets up AAPT, BUILD_TOOLS, SDK, NDK, ADB, UNAME

#=========================================================================

# Setting compile / linking flags

message(STATUS "Setting Compile Flags for ANDROID")
set(ANDROID_COMPILE_FLAGS "-ffunction-sections -Os -fdata-sections -Wall -fvisibility=hidden ")
set(ANDROID_COMPILE_FLAGS "${ANDROID_COMPILE_FLAGS} -Os -DANDROID -DAPPNAME=${APPNAME} -DANDROID_FULLSCREEN=y -DANDROIDVERSION=${ANDROIDVERSION} ")
set(CFLAGS_ARM64 "-m64 ")
message(STATUS)

message(STATUS "Setting Link Flags for ANDROID")
set(ANDROID_LINK_FLAGS "-Wl --gc-sections -s ") 
set(ANDROID_LINK_FLAGS "${ANDROID_LINK_FLAGS} -shared -uANativeActivity_onCreate ")
message(STATUS)

set(CMAKE_CXX_FLAGS "${ANDROID_COMPILE_FLAGS} -fPIC ")

#=========================================================================

set(LLVM_LIBC++ ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/sysroot/usr/lib)#Test if libc++ included was the wrong one
set(PROC_LIBS ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/sysroot/usr/lib)

set(CC_ARM64 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/aarch64-linux-android${ANDROIDVERSION}-clang)
set(CCXX_ARM64 ${CC_ARM64}++)

set(Larm64 ${CMAKE_BINARY_DIR}/lib/arm64-v8a)
set(Barm64 ${CMAKE_BINARY_DIR}/bin/arm64-v8a)
execute_process(COMMAND mkdir -p "${Larm64}")
execute_process(COMMAND mkdir -p "${Barm64}")
execute_process(COMMAND cp ${LLVM_LIBC++}/aarch64-linux-android/libc++_shared.so ${Larm64})
set(LLVM_LIBC++ ${Larm64}/libc++_shared.so)
set(CMAKE_C_COMPILER ${CC_ARM64})
set(CMAKE_CXX_COMPILER ${CCXX_ARM64})
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -v ${CFLAGS_ARM64} ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v ${CFLAGS_ARM64} ")
set(LIBLINK ${PROC_LIBS}/aarch64-linux-android/${ANDROIDVERSION})
set(INCL_PLATFORM aarch64-linux-android)

set(ANDROID_DIR ${CMAKE_SOURCE_DIR}/EngineCode/android)
set(app_glue "${NDK}/sources/android/native_app_glue/android_native_app_glue.c")

# NDK App Glue added directly to project to compile
set(LTA_FLYLIB "${PROJECT_NAME}_LTA") # Specifying name for avoiding name clashing when multiple builds 
add_library(${LTA_FLYLIB} SHARED 
    ${app_glue}  # NDK Native App Glue
    ${FLYLIB_SOURCE}) # Lib Sources

set_target_properties(${LTA_FLYLIB}
    PROPERTIES 
        ARCHIVE_OUTPUT_DIRECTORY "${Larm64}"
        LIBRARY_OUTPUT_DIRECTORY "${Larm64}"
        RUNTIME_OUTPUT_DIRECTORY "${Barm64}"
)

# Link to necessary NDK Directories
set(NDKINCLUDE ${NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include)

# Set Required includes from Android NDK
target_include_directories(${LTA_FLYLIB} PUBLIC 
    ${NDKINCLUDE} 
    ${NDKINCLUDE}/android
    ${NDKINCLUDE}/${INCL_PLATFORM}
    ${NDK}/sources/android/native_app_glue
    ${NDK}/sources/android/cpufeatures
)

# Include External libraries (header only, source code,...) to own project if needed

# Call build to external libraries

# Link to the built/prebuilt external libraries

# Link to Android Prebuilt Libraries
target_link_libraries(${LTA_FLYLIB} PUBLIC 
    ${LIBLINK}/libm.so
    ${LIBLINK}/libandroid.so 
    ${LIBLINK}/libEGL.so
    ${LIBLINK}/libGLESv3.so
    ${LIBLINK}/liblog.so
    ${LLVM_LIBC++}
    atomic
)