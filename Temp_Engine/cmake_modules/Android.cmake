# Android App Naming
set(APPNAME ${PROJ_NAME})
set(ORG_NAME "MarcTorresJimenez")
set(LABEL ${APPNAME})
set(APKFILE "${APPNAME}.apk")
set(PACKAGENAME "org.${ORG_NAME}.${APPNAME}")
set(ANDROID_GLUE "${.}/Android Specific/android_native_app_glue.c")

# Prepare Android build/compile/link commands
set(ANDROIDVERSION 24)
set(ANDROIDTARGET ${ANDROIDVERSION})
set(CFLAGS)
set(LDFLAGS)
list(APPEND LDFLAGS "-Wl,--gc-sections" -s)
list(APPEND CFLAGS -ffunction-sections -Os -fdata-sections -Wall -fvisibility=hidden)
set(ANDROID_FULLSCREEN y)
set(ADB adb)
set(UNAME $ENV{USER})
set(SRCS "${ENGINE_SOURCES}")


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
list(APPEND CFLAGS -Os -DAPPNAME=\"${APPNAME}\")
list(APPEND CFLAGS -DANDROID_FULLSCREEN=y)
list(APPEND CFLAGS -I${NDK}/sysroot/usr/include -I${NDK}/sysroot/usr/include/android -fPIC -DANDROIDVERSION=${ANDROIDVERSION})
message(STATUS)

# Linker Flags
message(STATUS "Setting linker flags for android")
list(APPEND LDFLAGS -lm) # -lEGL -landroid -llog)
list(APPEND LDFLAGS -shared -uANativeActivity_onCreate)
message(STATUS)

# Processor things, don't understand
message(STATUS "Setting Processor Architecture VarPaths")
set(CC_ARM64 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/aarch64-linux-android${ANDROIDVERSION}-clang++)
set(CC_ARM32 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/armv7a-linux-androideabi${ANDROIDVERSION}-clang++)
set(CC_x86 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/x86_64-linux-android${ANDROIDVERSION}-clang++)
set(CC_x86_64 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/x86_64-linux-android${ANDROIDVERSION}-clang++)

subdirlist(FF ${BUILD_TOOLS})
first_exists(FE "${FF}")
set(AAPT ${FE}/aapt)

# Set Targets for makecapk creation to work
set(TARGETS makecapk/lib/arm64-v8a/lib${APPNAME}.so makecapk/lib/armeabi-v7a/lib${APPNAME}.so)
#TARGETS += makecapk/lib/x86/lib$(APPNAME).so
#TARGETS += makecapk/lib/x86_64/lib$(APPNAME).so
set(CFLAGS_ARM64 -m64)
set(CFLAGS_ARM32 -m32) #-mfloat-abi=softfp  / default in arm32 is softp and it can't be found when compiling, sooooooo
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
# Ideally this could be passed as a Target to Makefile and have a target keystore that executes that, instead of here

# GENERATE CUSTOM TARGETS/CUSTOM COMMANDS FOR PROCESSOR ARCHITECTURES
# missing ARM64, ARM32, x86
# testing trying to get make to work with X64

# Make Sure all flags are setup properly

message(STATUS "CFLAGS: " "${CFLAGS}")
message(STATUS "X86 Compiler Flags: " "${CC_X86}")
message(STATUS "CFLAGS_X86: " "${CFLAGS_X86}")

set(HARD_LINK_1 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/sysroot/usr/lib)
set(x86Link i686-linux-android/${ANDROIDVERSION})
set(x86_64Link x86_64-linux-android/${ANDROIDVERSION})
set(arm32Link arm-linux-androideabi/${ANDROIDVERSION})
set(arm64Link aarch64-linux-android/${ANDROIDVERSION})

add_custom_target( x86lib${APPNAME}.so ALL
    SOURCES ${SRCS} ${ANDROID_GLUE}
    COMMAND chmod 755 -R ${CMAKE_SOURCE_DIR}
    COMMAND @echo "Creating Directory for x86 lib"
    COMMAND mkdir -p makecapk/lib/x86
    COMMAND @echo "Building x86 so file"
    # In Make, g++ is invoked directly by make with the rules
    COMMAND g++ ${CC_X86}
        ${CFLAGS} 
        ${CFLAGS_X86}
        -o $@ $^ # Don't know what this does really
        -L${HARD_LINK1}/${x86Link}
        -L${HARD_LINK1}/${x86Link}/libandroid.so
        -L${HARD_LINK1}/${x86Link}/liblog.so
        -L${HARD_LINK1}/${x86Link}/libEGL.so
        ${LDFLAGS}
)

add_custom_target( x86_64lib${APPNAME}.so ALL
    SOURCES ${SRCS} ${ANDROID_GLUE}
    COMMAND chmod 755 -R ${CMAKE_SOURCE_DIR}
    COMMAND @echo "Creating Directory for x86_64 lib"
    COMMAND mkdir -p makecapk/lib/x86_64
    COMMAND @echo "Building x86 so file"
    # In Make, g++ is invoked directly by make with the rules
    COMMAND g++ ${CC_X86_64}
        ${CFLAGS} 
        ${CFLAGS_X86_64}
        -o $@ $^ # Don't know what this does really
        -L${HARD_LINK1}/${x86_64Link}
        -L${HARD_LINK1}/${x86_64Link}/libandroid.so
        -L${HARD_LINK1}/${x86_64Link}/liblog.so
        -L${HARD_LINK1}/${x86_64Link}/libEGL.so
        ${LDFLAGS}
)

add_custom_target( arm32lib${APPNAME}.so ALL
    SOURCES ${SRCS} ${ANDROID_GLUE}
    COMMAND chmod 755 -R ${CMAKE_SOURCE_DIR}
    COMMAND @echo "Creating Directory for ARM32 lib"
    COMMAND mkdir -p makecapk/lib/armeabi-v7a
    COMMAND @echo "Building ARM32 so file"
    # In Make, g++ is invoked directly by make with the rules
    COMMAND @echo ${NDK}
    COMMAND arm-none-eabi-g++ ${CC_ARM32}
        ${CFLAGS} 
        ${CFLAGS_ARM32}
        -o $@ $^ # Don't know what this does really
        -L${HARD_LINK1}/${arm32Link}
        -L${HARD_LINK1}/${arm32Link}/libandroid.so
        -L${HARD_LINK1}/${arm32Link}/liblog.so
        -L${HARD_LINK1}/${arm32Link}/libEGL.so
        ${LDFLAGS}
)

add_custom_target(PROCESSOR_LIBS ALL
    DEPENDS 
        x86lib${APPNAME}.so
        x86_64lib${APPNAME}.so
        arm32lib${APPNAME}.so
        # Add other architectures (copypaste really)
    COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/x86lib${APPNAME}.so lib${APPNAME}.so
    COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/lib${APPNAME}.so ./makecapk/lib/x86

    COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/x86_64lib${APPNAME}.so lib${APPNAME}.so
    COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/lib${APPNAME}.so ./makecapk/lib/x86_64

    COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/arm32lib${APPNAME}.so lib${APPNAME}.so
    COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/lib${APPNAME}.so ./makecapk/lib/armeabi-v7a
)

add_custom_target( AndroidManifest.xml ALL
    COMMAND @echo "Creating Manifest"
    COMMAND rm -rf AndroidManifest.xml
    COMMAND PACKAGENAME=${PACKAGENAME}
    COMMAND ANDROIDVERSION=${ANDROIDVERSION}
    COMMAND ANDROIDTARGET=${ANDROIDTARGET}
    COMMAND APPNAME=${APPNAME}
    COMMAND LABEL=${LABEL} 
    COMMAND envsubst '${ANDROIDTARGET} ${ANDROIDVERSION} ${APPNAME} ${PACKAGENAME} ${LABEL}'
    COMMAND @echo "Label Pass"
    COMMAND < AndroidManifest.xml.template > AndroidManifest.xml            
    COMMAND @echo "Finished Manifest"
)

add_custom_target( manifest BYPRODUCTS COMMAND AndroidManifest.xml)

add_custom_target( makecapk.apk ALL
    COMMAND ${PROCESSOR_LIBS}
    COMMAND ${ASSETS} 
    COMMAND AndroidManifest.xml
    COMMAND mkdir -p makecapk/assets
    #COMMAND cp -r 
    # Somewhere in the package commands/flags, we require addition of -S [dir] for the asset sources original place
    #[RAWDRAW] We're really cutting corners.  You should probably use resource files.. Replace android:label="@string/app_name" and add a resource file.
    #[RAWDRAW] Then do this -S Sources/res on the aapt line.
    #[RAWDRAW] For icon support, add -S makecapk/res to the aapt line.  also,  android:icon="@mipmap/icon" to your application line in the manifest.
    #[RAWDRAW] If you want to strip out about 800 bytes of data you can remove the icon and strings.

    COMMAND ${AAPT} package -f -F temp.apk -I ${ANDROIDSDK}/platforms/android-${ANDROIDVERSION}/android.jar -M AndroidManifest.xml -A makecapk/assets -v --target-sdk-version ${ANDROIDTARGET}
    COMMAND cd makecapk & zip -D0r ../makecapk.apk
    COMMAND jarsigner -sigalg SHA1withRSA -digestalg SHA1 -verose -keystore ${KEYSTOREFILE} -storepass ${STOREPASS} makecapk.apk ${ALIASNAME}
    COMMAND rm -rf ${APKFILE}
    COMMAND ${BUILD_TOOLS}/zipalign -v 4 makecapk.apk ${APKFILE}
    COMMAND rm -rg temp.apk
    COMMAND rm -rf makecapk.apk
    COMMAND @ls -l ${APKFILE}    
)

add_custom_target( uninstall
    COMMAND (${ADB} uninstall ${PACKAGENAME})||true 
)

add_custom_target( push
    COMMAND makecapk.apk
    COMMAND @echo "Installing " ${PACKAGENAME}
    COMMAND ${ADB} install -r ${APKFILE}
)


#add_custom_target( run
#    COMMAND push
#    COMMAND $(eval ACTIVITYNAME:=$(shell ))
#)

#add_custom_target( clean
#    COMMAND rm -rf temp.apk makecapk.apk makecapk ${APKFILE}
#)