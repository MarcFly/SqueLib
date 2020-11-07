add_compile_definitions(ANDROID)

# Android App Naming
set(APPNAME ${PROJ_NAME})
set(ORG_NAME "MarcTorresJimenez")
set(LABEL ${PROJ_NAME})
set(APKFILE "${APPNAME}.apk")
set(PACKAGENAME "org.${ORG_NAME}.${APPNAME}")
set(ANDROIDVERSION 29)
set(ANDROIDTARGET ${ANDROIDVERSION})
set(UNAME $ENV{USER})

set(APP_PLATFORM ANDROIDVERSION)

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

# Setting compile / linking flags

message(STATUS "Setting Compile Flags for ANDROID")
set(ANDROID_COMPILE_FLAGS "-ffunction-sections -Os -fdata-sections -Wall -fvisibility=hidden")
set(ANDROID_COMPILE_FLAGS "${ANDROID_COMPILE_FLAGS} -Os -DANDROID -DAPPNAME=\"${APPNAME}\" -DANDROID_FULLSCREEN -DANDROIDVERSION=${ANDROIDVERSION}")
set(CFLAGS_ARM64 "-m64")
set(CFLAGS_ARM32 "-march=armv7-a -mfloat-abi=softfp -m32")
set(CFLAGS_x86 "-march=i686 -mtune=intel -mssse3 -mfpmath=sse -m32")
set(CFLAGS_x86_64 "-march=x86-64 -msse4.2 -mpopcnt -m64 -mtune=intel")

message(STATUS)

message(STATUS "Setting Link Flags for ANDROID")
set(ANDROID_LINK_FLAGS "-Wl --gc-sections -s")
set(ANDROID_LINK_FLAGS "${ANDROID_LINK_FLAGS} -lm -lEGL -landroid -llog")
set(ANDROID_LINK_FLAGS "${ANDROID_LINK_FLAGS} -shared -uANativeActivity_onCreate")
message(STATUS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ANDROID_COMPILE_FLAGS} -fPIC")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ANDROID_LINK_FLAGS}")

#=========================================================================

# Setup for creating the keystore file
if(NOT ALIASNAME)
    message(STATUS "Setting up aliasname")
    set(ALIASNAME standkey)
endif()
if(STOREPASS)
    message(FATAL_ERROR "Password/Storepass for key not passed")
endif()
set(DNAME "CN=marcfly.github.io, OU=ID, O=Example?, L=Torres, S=Marc, C=ES")
if(NOT STOREPASS)
    set(STOREPASS ${ALIASNAME})
endif()
set(KEYSTOREFILE "./my-release-key.keystore")

# This is currently set to generate a new keyfile everytime
execute_process(COMMAND keytool -genkey -v -keystore ${KEYSTOREFILE} -alias ${ALIASNAME} -keyalg RSA -keysize 2048 -validity 10000 -storepass ${STOREPASS} -keypass ${STOREPASS} -dname ${DNAME})

#=========================================================================

# Generate lib folders if they don't exists
execute_process(COMMAND mkdir -p makecapk)
execute_process(COMMAND mkdir -p makecapk/lib)

SET(LLVM_LIBC++ ${NDK}/sources/cxx-stl/llvm-libc++/libs)
set(PROC_LIBS ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/sysroot/usr/lib)

set(CXX_ARM64 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/aarch64-linux-android${ANDROIDVERSION}-clang++)
set(CXX_ARM32 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/armv7a-linux-androideabi${ANDROIDVERSION}-clang++)
set(CXX_x86 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/x86_64-linux-android${ANDROIDVERSION}-clang++)
set(CXX_x86_64 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/x86_64-linux-android${ANDROIDVERSION}-clang++)


if(ARM64)
    execute_process(COMMAND mkdir -p makecapk/lib/arm64-v8a)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ./makecapk/lib/arm64-v8a)
    execute_process(COMMAND cp ${LLVM_LIBC++}/arm64-v8a/libc++_shared.so ./makecapk/lib/arm64-v8a)
    set(CMAKE_CXX_COMPILER ${CXX_ARM64})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CFLAGS_ARM64} -L${PROC_LIBS}/aarch64-linux-android/${ANDROIDVERSION}")
elseif(ARM32)
    execute_process(COMMAND mkdir -p makecapk/lib/armeabi-v7a)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ./makecapk/lib/armeabi-v7a)
    execute_process(COMMAND cp ${LLVM_LIBC++}/armeabi-v7a/libc++_shared.so ./makecapk/lib/armeabi-v7a)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CFLAGS_ARM64} -L${PROC_LIBS}/aarch64-linux-android/${ANDROIDVERSION}/")
elseif(X86)
    execute_process(COMMAND mkdir -p makecapk/lib/x86)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ./makecapk/lib/x86)
    execute_process(COMMAND cp ${LLVM_LIBC++}/x86/libc++_shared.so ./makecapk/lib/x86)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CFLAGS_ARM64} -L${PROC_LIBS}/aarch64-linux-android/${ANDROIDVERSION}/")
elseif(X64)
    execute_process(COMMAND mkdir -p makecapk/lib/x86_64)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ./makecapk/lib/x86_64)
    execute_process(COMMAND cp ${LLVM_LIBC++}/x86_64/libc++_shared.so ./makecapk/lib/x86_64)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CFLAGS_ARM64} -L${PROC_LIBS}/aarch64-linux-android/${ANDROIDVERSION}/")
endif()

add_library(app_glue STATIC "${NDK}/sources/android/native_app_glue/android_native_app_glue.c")
target_include_directories(app_glue PUBLIC ${NDK}/sources/android/native_app_glue)
set_target_properties(app_glue PROPERTIES LINKER_LANGUAGE CXX)

add_library(TempEngine SHARED "${ENGINE_SOURCES}")
target_include_directories(TempEngine PUBLIC ${NDK}/sysroot/usr/include/android)

#-I$(NDK)/sysroot/usr/include -I$(NDK)/sysroot/usr/include/android -fPIC

target_link_libraries(TempEngine app_glue)