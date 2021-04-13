# Android Version will be 29 for now (only I can test)
macro(set_app_properties appName orgName androidVer)
    set(APPNAME '"${appName}"')
    set(ORG_NAME "${orgName}")
    set(LABEL ${appName})
    set(APKFILE "${appName}.apk")
    set(PACKAGENAME "org.${orgName}.${appName}")
    set(ANDROIDVERSION ${androidVer})
    set(ANDROIDTARGET ${androidVer})
endmacro()
#------------------------------------------------------------------------------------------------
# SETUP ANDROID SPECIFIC COMPILE OPTIONS + PROCESSES
#------------------------------------------------------------------------------------------------
macro(set_android_link_flags)
    set(ANDROID_LINK_FLAGS " -shared -uANativeActivity_onCreate -Wl --gc-sections -s ") # REQUIRED FLAGS
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ANDROID_LINK_FLAGS} ${ARGN} ")
    message(STATUS "Setting Link Flags for ANDROID: ${CMAKE_EXE_LINKER_FLAGS}")
    message(STATUS)
endmacro()

macro(set_android_compile_flags)
    if(DEBUG OR NDK_DEBUG)
        set(ANDROID_OPT "-O0")
    else()
        set(ANDROID_OPT "-Ofast")
    endif()
    set(ANDROID_COMPILE_FLAGS "-ffunction-sections ${ANDROID_OPT} -fdata-sections -v")
    set(ANDROID_COMPILE_FLAGS "${ANDROID_COMPILE_FLAGS} -DANDROID -DAPPNAME=${APPNAME} -DANDROID_FULLSCREEN=y -DANDROIDVERSION=${ANDROIDVERSION} ")
    set(CFLAGS_ARM64 "-m64 ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ANDROID_COMPILE_FLAGS} -fPIC ${CFLAGS_ARM64} ${ARGN} ")
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")
    message(STATUS "Setting Compile Flags for ANDROID: ${CMAKE_CXX_FLAGS}")
    message(STATUS)
endmacro()

macro(set_android_compiler NDK_PATH OS_USED ANDR_V)
    set(CMAKE_C_COMPILER "${NDK_PATH}/toolchains/llvm/prebuilt/${OS_USED}/bin/aarch64-linux-android${ANDR_V}-clang")
    set(CMAKE_CXX_COMPILER "${CMAKE_C_COMPILER}++")
    if(WIN32)
        set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER}.cmd")
        set(CMAKE_CXX_COMPILER "${CMAKE_CXX_COMPILER}.cmd")
    endif()
    
endmacro()

macro(link_android_libc target NDK_PATH OS_USED ANDR_V)
    set(ARM_LIBS ${NDK_PATH}/toolchains/llvm/prebuilt/${OS_USED}/sysroot/usr/lib)
    execute_process(COMMAND cp ${ARM_LIBS}/aarch64-linux-android/libc++_shared.so ${CMAKE_BINARY_DIR}/makecapk/lib/arm64-v8a)
    set(LIBLINK ${ARM_LIBS}/aarch64-linux-android/${ANDR_V})
    target_link_libraries(${target} PUBLIC 
        ${LIBLINK}/../libc++_shared.so
        atomic
    )
endmacro()

macro(link_android_all_libs target NDK_PATH OS_USED ANDR_V)
    set(ARM_LIBS ${NDK_PATH}/toolchains/llvm/prebuilt/${OS_USED}/sysroot/usr/lib)
    execute_process(COMMAND cp ${ARM_LIBS}/aarch64-linux-android/libc++_shared.so ${CMAKE_BINARY_DIR}/makecapk/lib/arm64-v8a)
    set(LIBLINK ${ARM_LIBS}/aarch64-linux-android/${ANDR_V})
    target_link_libraries(${target} PUBLIC 
        ${LIBLINK}/../libc++_shared.so
        ${LIBLINK}/libm.so
        ${LIBLINK}/libnativewindow.so
        ${LIBLINK}/libandroid.so        
        ${LIBLINK}/libEGL.so
        ${LIBLINK}/libGLESv3.so
        ${LIBLINK}/liblog.so
        ${LIBLINK}/libOpenSLES.so
        atomic
    )
endmacro()

macro(include_android target NDK_PATH OS_USED)
    set(NDKINCLUDE ${NDK_PATH}/toolchains/llvm/prebuilt/${OS_USED}/sysroot/usr/include)
    target_include_directories(${target} PUBLIC 
        ${NDKINCLUDE}
        ${NDKINCLUDE}/aarch64-linux-android
        ${NDK_PATH}/sources/android/native_app_glue
        ${NDK_PATH}/sources/android/cpufeatures
    )
endmacro()