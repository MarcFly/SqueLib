
macro(set_app_properties AppName OrgName AndroidVersion)
    set(APPNAME '"${AppName}"')
    set(ORG_NAME "${OrgName}")
    set(LABEL ${AppName})
    set(APKFILE "${AppName}.apk")
    set(PACKAGENAME "org.${OrgName}.AppName")
    set(ANDROIDVERSION ${AndroidVersion})
    set(ANDROIDTARGET ${AndroidVersion})
    set(ADB adb)
endmacro()
#------------------------------------------------------------------------------------------------
# SETUP ANDROID SPECIFIC COMPILE OPTIONS + PROCESSES
#------------------------------------------------------------------------------------------------
macro(set_android_link_flags)
    set(ANDROID_LINK_FLAGS " -s -shared -uANativeActivity_onCreate ") # REQUIRED FLAGS
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ANDROID_LINK_FLAGS} ${ARGN} ")
    message(STATUS "Setting Link Flags for ANDROID: ${CMAKE_EXE_LINKER_FLAGS}")
    message(STATUS)
endmacro()

macro(set_android_compile_flags)
    set(ANDROID_COMPILE_FLAGS "-ffunction-sections -Os -fdata-sections -Wall ")
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
        ${LIBLINK}/libandroid.so 
        ${LIBLINK}/libEGL.so
        ${LIBLINK}/libGLESv3.so
        ${LIBLINK}/liblog.so
        atomic
    )
endmacro()

macro(include_android target NDK_PATH)
    set(NDKINCLUDE ${NDK_PATH}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include)
    target_include_directories(${target} PUBLIC 
        ${NDKINCLUDE} 
        ${NDKINCLUDE}/android
        ${NDKINCLUDE}/${ANDROID_PLATFORM}
        ${NDK_PATH}/sources/android/native_app_glue
        ${NDK_PATH}/sources/android/cpufeatures
    )
endmacro()