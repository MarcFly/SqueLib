set(ANDROIDVERSION 29) # Only one I can test at home
# Will only support arm64, going forward it won't be needed to support x32

# Sets up AAPT, BUILD_TOOLS, SDK, NDK, ADB, UNAME
include(${FL_cmake}/FindAndroidSDKVars.cmake) 
#------------------------------------------------------------------------------------------------
# SETUP ANDROID SPECIFIC COMPILE OPTIONS + PROCESSES
#------------------------------------------------------------------------------------------------
# When compiling it does not get defined directly by compiler, it defines host platform
add_compile_definitions(ANDROID)
set(CMAKE_CXX_EXTENSIONS OFF)

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

set(ANDROID_PREBUILT ${NDK}/toolchains/llvm/prebuilt/${OS_NAME})
set(ARM_LIBS ${ANDROID_PREBUILT}/sysroot/usr/lib)
set(ANDROID_PLATFORM aarch64-linux-android)
set(CC_ARM64 ${ANDROID_PREBUILT}/bin/aarch64-linux-android${ANDROIDVERSION}-clang)
set(CCXX_ARM64 ${CC_ARM64}++)

set(FL_OutputFolder ${CMAKE_BINARY_DIR}/makecapk/lib/arm64-v8a)
execute_process(COMMAND mkdir -p "${FL_OutputFolder}")
execute_process(COMMAND cp ${ARM_LIBS}/${ANDROID_PLATFORM}/libc++_shared.so ${FL_OutputFolder})
set(LLVM_LIBC++ ${FL_OutputFolder}/libc++_shared.so)
set(CMAKE_C_COMPILER ${CC_ARM64})
set(CMAKE_CXX_COMPILER ${CCXX_ARM64})
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -v ${CFLAGS_ARM64} ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v ${CFLAGS_ARM64} ")
set(LIBLINK ${ARM_LIBS}/${ANDROID_PLATFORM}/${ANDROIDVERSION})
#------------------------------------------------------------------------------------------------
# SETUP PROJECT OUTPUT
#------------------------------------------------------------------------------------------------
# NDK App Glue added directly to project to compile
set(app_glue "${NDK}/sources/android/native_app_glue/android_native_app_glue.c")
add_library(FlyLib SHARED 
    ${app_glue}  # NDK Native App Glue
    ${FLYLIB_SOURCE}) # Lib Sources

set_target_properties(FlyLib
    PROPERTIES 
        ARCHIVE_OUTPUT_DIRECTORY "${FL_OutputFolder}"
        LIBRARY_OUTPUT_DIRECTORY "${FL_OutputFolder}"
        RUNTIME_OUTPUT_DIRECTORY "${FL_OutputFolder}"
)
#------------------------------------------------------------------------------------------------
# PLATFORM SPECIFIC INCLUDE/LINKS
#------------------------------------------------------------------------------------------------
# ANDROID NDK Includes
set(NDKINCLUDE ${NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include)
target_include_directories(FlyLib PRIVATE 
    ${NDKINCLUDE} 
    ${NDKINCLUDE}/android
    ${NDKINCLUDE}/${ANDROID_PLATFORM}
    ${NDK}/sources/android/native_app_glue
    ${NDK}/sources/android/cpufeatures
)
# ANDROID Native/Prebuilt Libraries
target_link_libraries(FlyLib PRIVATE
    ${LIBLINK}/libm.so
    ${LIBLINK}/libandroid.so 
    ${LIBLINK}/libEGL.so
    ${LIBLINK}/libGLESv3.so
    ${LIBLINK}/liblog.so
    ${LLVM_LIBC++}
    atomic
)