#------------------------------------------------------------------------------------------------
# APP NAMING AND VARIABLES TO SETUP AndroidManifest.xml
#------------------------------------------------------------------------------------------------
add_compile_definitions(ANDROID)
set(CMAKE_CXX_EXTENSIONS OFF)

# Android App Naming
set(APPNAME '"TempEngine"')
set(ORG_NAME "MarcTorresJimenez")
set(LABEL TempEngine)
set(APKFILE "TempEngine.apk")
set(PACKAGENAME "org.${ORG_NAME}.TempEngine")
set(ANDROIDVERSION 29)
set(ANDROIDTARGET ${ANDROIDVERSION})

set(ADB adb)
#------------------------------------------------------------------------------------------------
# Find NDK / BUILD_TOOLS / ANDROIDSDK directories
#------------------------------------------------------------------------------------------------
include(${TE_cmake}/FindAndroidSDKVars.cmake)
#------------------------------------------------------------------------------------------------
# SETUP ANDROID SPECIFIC COMPILE OPTIONS + PROCESSES
#------------------------------------------------------------------------------------------------
# Linker Options
set(ANDROID_LINK_FLAGS " -s -shared -uANativeActivity_onCreate ")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ANDROID_LINK_FLAGS} ")
message(STATUS "Setting Link Flags for ANDROID ${ANDROID_LINK_FLAGS}")
message(STATUS)

# Compiler Options
set(ANDROID_COMPILE_FLAGS "-ffunction-sections -Os -fdata-sections -Wall ")
set(ANDROID_COMPILE_FLAGS "${ANDROID_COMPILE_FLAGS} -DANDROID -DAPPNAME=${APPNAME} -DANDROID_FULLSCREEN=y -DANDROIDVERSION=${ANDROIDVERSION} ")
set(CFLAGS_ARM64 "-m64 ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ANDROID_COMPILE_FLAGS} -fPIC ")
message(STATUS "Setting Compile Flags for ANDROID: ${CMAKE_CXX_FLAGS}")
message(STATUS)

set(ANDROID_PREBUILT ${NDK}/toolchains/llvm/prebuilt/${OS_NAME})
set(ARM_LIBS ${ANDROID_PREBUILT}/sysroot/usr/lib)
set(ANDROID_PLATFORM aarch64-linux-android)
set(CC_ARM64 "${ANDROID_PREBUILT}/bin/aarch64-linux-android${ANDROIDVERSION}-clang")
set(CCXX_ARM64 "${CC_ARM64}++")

message(STATUS "Setting up compiler")
set(TE_OutputFolder ${CMAKE_BINARY_DIR}/makecapk/lib/arm64-v8a)
message(STATUS "Building folder for output")
execute_process(COMMAND mkdir -p ${TE_OutputFolder})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TE_OutputFolder})
message(STATUS "Copying to Folder")
execute_process(COMMAND cp ${ARM_LIBS}/${ANDROID_PLATFORM}/libc++_shared.so ${TE_OutputFolder})
set(LLVM_LIBC++ ${TE_OutputFolder}/libc++_shared.so)
set(CMAKE_C_COMPILER ${CC_ARM64})
set(CMAKE_CXX_COMPILER ${CCXX_ARM64})
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -v ${CFLAGS_ARM64} ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v ${CFLAGS_ARM64} ")
set(LIBLINK ${ARM_LIBS}/${ANDROID_PLATFORM}/${ANDROIDVERSION})
#------------------------------------------------------------------------------------------------
# SETUP PROJECT OUTPUT
#------------------------------------------------------------------------------------------------
add_library(TempEngine SHARED "${ENGINE_SRC}")

set_target_properties(TempEngine
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive"
        LIBRARY_OUTPUT_DIRECTORY "${TE_OutputFolder}"
        RUNTIME_OUTPUT_DIRECTORY "${TE_OutputFolder}")
target_link_libraries(TempEngine PUBLIC ${LLVM_LIBC++}) 
target_link_options(TempEngine PUBLIC -s -uANativeActivity_onCreate -shared)

target_c
