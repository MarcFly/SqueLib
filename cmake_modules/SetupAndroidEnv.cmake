# Android Version will be 29 for now (only I can test)
macro(set_app_properties appName orgName)
    set(APPNAME '"${appName}"')
    set(ORG_NAME "${orgName}")
    set(LABEL ${appName})
    set(APKFILE "${appName}.apk")
    set(PACKAGENAME "org.${orgName}.${appName}")
    set(ANDROIDVERSION ${ANDROID_VERSION})
    set(ANDROIDTARGET ${ANDROID_VERSION})
endmacro()
#------------------------------------------------------------------------------------------------
# SETUP ANDROID SPECIFIC COMPILE OPTIONS + PROCESSES
#------------------------------------------------------------------------------------------------
macro(link_android_libc target)
    set(ARM_LIBS ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/sysroot/usr/lib)
    execute_process(COMMAND cp ${ARM_LIBS}/aarch64-linux-android/libc++_shared.so ${CMAKE_BINARY_DIR}/makecapk/lib/arm64-v8a)
    set(LIBLINK ${ARM_LIBS}/aarch64-linux-android/${ANDROID_VERSION})
    target_link_libraries(${target} PUBLIC 
        ${LIBLINK}/../libc++_shared.so
        atomic
    )
endmacro()

macro(link_android_all_libs target)
    set(ARM_LIBS ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/sysroot/usr/lib)
    set(LIBLINK ${ARM_LIBS}/aarch64-linux-android/${ANDROID_VERSION})
    target_link_libraries(${target} PUBLIC 
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

macro(include_android target)
    set(NDKINCLUDE ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/sysroot/usr/include)
    target_include_directories(${target} PUBLIC 
        ${NDKINCLUDE}
        ${NDKINCLUDE}/aarch64-linux-android
        ${NDK}/sources/android/native_app_glue
        ${NDK}/sources/android/cpufeatures
    )
    
endmacro()