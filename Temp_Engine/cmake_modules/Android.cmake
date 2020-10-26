# Android App Naming
set(ORG_NAME "MarcTorresJimenez")
set(LABEL ${PROJ_NAME})
set(APKFILE "${PROJ_NAME}.apk")
set(PACKAGENAME "org.${ORG_NAME}.${PROJ_NAME}")
set(ANDROID_GLUE "./Android Specific/android_native_app_glue")

# Prepare Android build/compile/link commands
set(ANDROIDVERSION 22)
set(ANDROIDTARGET ${ANDROIDVERSION})
list(APPEND ${CMAKE_EXE_LINKER_FLAGS} -Wl --gc-sections -s)
list(APPEND ${COMPILE_FLAGS} -ffunction-sections -Os -fdata-sections -Wall -fvisibility=hidden)
set(ANDROID_FULLSCREEN y)
set(ADB adb)
set(UNAME $ENV{USER})
#set(ANDROIDSRCS)


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
    first_exists(NDK "${NDK_DIRS}")
endif()

# Search for Build Tools inside SDK
first_exists(BUILD_TOOLS ${ANDROIDSDK}/build-tools)

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

# Compiler Flags
message(STATUS "Setting Compile Flags for Android")
list(APPEND ${COMPILE_FLAGS} -Os -DAPPNAME=\"${PROJ_NAME}\")
list(APPEND ${COMPILE_FLAGS} -DANDROID_FULLSCREEN=y)
list(APPEND ${COMPILE_FLAGS} -I${NDK}/sysroot/usr/include -I${NDK}/sysroot/usr/include/android -fPIC -DANDROIDVERSION=$(ANDROIDVERSION))
message(STATUS)

# Linker Flags
message(STATUS "Setting linker flags for android")
list(APPEND ${CMAKE_EXE_LINKER_FLAGS} -lm -landroid -llog)
list(APPEND ${CMAKE_EXE_LINKER_FLAGS} -shared -uANativeActivity_onCreate)
message(STATUS)

# Processor things, don't understand
message(STATUS "Setting Processor Architecture VarPaths")
set(CC_ARM64 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/aarch64-linux-android${ANDROIDVERSION}-clang)
    set(CC_ARM32 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/armv7a-linux-androideabi${ANDROIDVERSION}-clang)
set(CC_x86 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/x86_64-linux-android${ANDROIDVERSION}-clang)
set(CC_x86_64 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/x86_64-linux-android${ANDROIDVERSION}-clang)
message(STATUS)

# Set Targets for makecapk to work
set(TARGETS makecapk/lib/arm64-v8a/lib${PROJ_NAME}.so makecapk/lib/armeabi-v7a/lib${PROJ_NAME}.so)
set(CFLAGS_ARM64 -m64)
set(CFLAGS_ARM32 -mfloat-ai=softfp -m32)
set(CFLAGS_x86 -march=i686 -mtune=intel -mssse3 -mfpmath=sse -m32)
set(CFLAGS_x86_64 -march=x86-64 -msse4.2 -mpopcnt -m64 -mtune=intel)

# Setup for creating the keystore file
if(NOT ${STOREPASS})
    message(FATAL_ERROR "Password/Storepass for key not passed")
endif()
set(DNAME "CN=marcfly.github.io, OU=ID, O=Example?, L=Torres, S=Marc, C=ES")
set(ALIASNAME standkey)
if(NOT ${STOREPASS})
    set(STOREPASS standkey)
endif()
set(KEYSTOREFILE "./my-release-key.keystore")

# This is currently set to generate a new keyfile everytime
execute_process(COMMAND keytool -genkey -v -keystore ${KEYSTOREFILE} -alias ${ALIASNAME} -keyalg RSA -keysize 2048 -validity 10000 -storepass ${STOREPASS} -keypass ${STOREPASS} -dname ${DNAME})
