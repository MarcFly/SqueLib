add_compile_definitions(ANDROID)

# Android App Naming
set(APPNAME ${PROJ_NAME})
set(ORG_NAME "MarcTorresJimenez")
set(LABEL ${PROJ_NAME})
set(APKFILE "${APPNAME}.apk")
set(PACKAGENAME "org.${ORG_NAME}.${APPNAME}")

set(GLUE_DIR "${.}/EngineCode/android/glue")
include_directories(${GLUE_DIR})
file(GLOB_RECURSE ANDROID_GLUE "${GLUE_DIR}/*.c" "${GLUE_DIR}/.h")

# Prepare Android build/compile/link commands

set(CFLAGS)
set(LDFLAGS)
list(APPEND LDFLAGS "-Wl,--gc-sections" -s)
list(APPEND CFLAGS -ffunction-sections -Os -fdata-sections -Wall -fvisibility=hidden) # Added verbosity to get more info on compilation (issues on files and so)
set(ANDROID_FULLSCREEN y)
set(ADB adb)
set(UNAME $ENV{USER})
set(SRCS ${.}/EngineCode/test.cpp)# "${ENGINE_SOURCES}")


# Set Route to find SDKs
set(OS_NAME linux-x86_64)

if(Windows)
    set(OS_NAME windows-x86_64)
endif(Windows)

if(Darwin)
    set(OS_NAME darwin-x86-64)
endif(Darwin)

message(STATUS "Building from " ${OS_NAME})



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
list(APPEND LDFLAGS -lm) # -lEGL -landroid -llog) # Libraries are currently hard linked to the version found, instead of added from link path which does not seem to work
list(APPEND LDFLAGS -shared -uANativeActivity_onCreate)
message(STATUS)

# Processor things, don't understand
message(STATUS "Setting Processor Architecture VarPaths")
set(CC_ARM64 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/aarch64-linux-android${ANDROIDVERSION}-clang++)
set(CC_ARM32 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/armv7a-linux-androideabi${ANDROIDVERSION}-clang++)
set(CC_x86 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/x86_64-linux-android${ANDROIDVERSION}-clang++)
set(CC_x86_64 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/x86_64-linux-android${ANDROIDVERSION}-clang++)

set(AAPT ${BUILD_TOOLS}/aapt)

# Set Targets for makecapk creation to work
set(TARGETS makecapk/lib/arm64-v8a/lib${APPNAME}.so makecapk/lib/armeabi-v7a/lib${APPNAME}.so)
#TARGETS += makecapk/lib/x86/lib$(APPNAME).so
#TARGETS += makecapk/lib/x86_64/lib$(APPNAME).so

set(CFLAGS_ARM64 -march=armv8-a -m64)
set(CFLAGS_ARM32  -march=armv7-a -mfloat-abi=softfp -m32)
set(CFLAGS_x86 -march=i686 -mtune=intel -mssse3 -mfpmath=sse -m32)
set(CFLAGS_x86_64 -march=x86-64 -msse4.2 -mpopcnt -m64 -mtune=intel)

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
# Ideally this could be passed as a Target to Makefile and have a target keystore that executes that, instead of here

# GENERATE CUSTOM TARGETS/CUSTOM COMMANDS FOR PROCESSOR ARCHITECTURES
# missing ARM64, ARM32, x86
# testing trying to get make to work with X64

# Make Sure all flags are setup properly

message(STATUS "CFLAGS: " "${CFLAGS}")
message(STATUS "X86 Compiler Flags: " "${CC_X86}")
message(STATUS "CFLAGS_X86: " "${CFLAGS_X86}")

set(PROC_LIBS ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/sysroot/usr/lib)
set(x86 i686-linux-android)
set(x64 x86_64-linux-android)
set(arm32 arm-linux-androideabi)
set(arm64 aarch64-linux-android)

add_custom_target( x86lib${APPNAME}.so
    SOURCES "${SRCS}" "${ANDROID_GLUE}"
    COMMAND chmod 755 -R ${CMAKE_SOURCE_DIR}
    COMMAND @echo "Creating Directory for x86 lib"
    COMMAND mkdir -p makecapk/lib/x86
    COMMAND @echo "Building x86 so file"
    # In Make, g++ is invoked directly by make with the rules
    COMMAND g++${CC_X86}
        ${CFLAGS} 
        ${CFLAGS_X86}
        -o $@ $^ # Don't know what this does really
        -L${PROC_LIBS}/${x86}/${ANDROIDVERSION}/libandroid.so
        -L${PROC_LIBS}/${x86}/${ANDROIDVERSION}/liblog.so
        -L${PROC_LIBS}/${x86}/${ANDROIDVERSION}/libEGL.so
        ${LDFLAGS}
)

add_custom_target( x86_64lib${APPNAME}.so
    SOURCES "${SRCS}" "${ANDROID_GLUE}"
    COMMAND chmod 755 -R ${CMAKE_SOURCE_DIR}
    COMMAND @echo "Creating Directory for x86_64 lib"
    COMMAND mkdir -p makecapk/lib/x86_64
    COMMAND @echo "Building x86 so file"
    # In Make, g++ is invoked directly by make with the rules
    COMMAND g++ ${CC_X86_64}
        ${CFLAGS} 
        ${CFLAGS_X86_64}
        -o $@ $^ # Don't know what this does really
        -L${PROC_LIBS}/${x64}/${ANDROIDVERSION}/libandroid.so
        -L${PROC_LIBS}/${x64}/${ANDROIDVERSION}/liblog.so
        -L${PROC_LIBS}/${x64s}/${ANDROIDVERSION}/libEGL.so
        ${LDFLAGS}
)

add_custom_target( arm32lib${APPNAME}.so
    SOURCES "${SRCS}" "${ANDROID_GLUE}"
    COMMAND chmod 755 -R ${CMAKE_SOURCE_DIR}
    COMMAND @echo "Creating Directory for ARM32 lib"
    COMMAND mkdir -p makecapk/lib/armeabi-v7a
    COMMAND @echo "Building ARM32 so file"
    # In Make, g++ is invoked directly by make with the rules
    # I don't know why but for linux base g++ will compile SO file with binaries
    # for arm I have to call the clan++ executable/file as one and will call a crosscompiler instead
    COMMAND ${CC_ARM32}
        ${CFLAGS} 
        ${CFLAGS_ARM32}
        -o $@ $^ # Don't know what this does really
        -L${PROC_LIBS}/${arm32}/${ANDROIDVERSION}/libandroid.so
        -L${PROC_LIBS}/${arm32}/${ANDROIDVERSION}/liblog.so
        -L${PROC_LIBS}/${arm32}/${ANDROIDVERSION}/libEGL.so
        ${LDFLAGS}
    COMMAND -ls
)

add_custom_target( arm64lib${APPNAME}.so
    SOURCES "${SRCS}" "${ANDROID_GLUE}"
    COMMAND chmod 755 -R ${CMAKE_SOURCE_DIR}
    COMMAND @echo "Creating Directory for ARM32 lib"
    COMMAND mkdir -p makecapk/lib/arm64-v8a
    COMMAND @echo "Building ARM32 so file"
    COMMAND ${CC_ARM64}
        ${CFLAGS} 
        ${CFLAGS_ARM64}
        -o $@ $^ # Don't know what this does really
        -L${PROC_LIBS}/${arm64}/${ANDROIDVERSION}/libandroid.so
        -L${PROC_LIBS}/${arm64}/${ANDROIDVERSION}/liblog.so
        -L${PROC_LIBS}/${arm64}/${ANDROIDVERSION}/libEGL.so
        ${LDFLAGS}
    COMMAND -ls
)

SET(LLVM_LIBC++ ${NDK}/sources/cxx-stl/llvm-libc++/libs)


add_custom_target(PROCESSOR_LIBS ALL
    DEPENDS # Make sure the so files are compiled
        x86lib${APPNAME}.so
        x86_64lib${APPNAME}.so
        arm32lib${APPNAME}.so
        arm64lib${APPNAME}.so

    # Rename and move the files into the lib folders in makecapk.apk
    COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/x86lib${APPNAME}.so lib${APPNAME}.so
    COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/lib${APPNAME}.so ./makecapk/lib/x86

    COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/x86_64lib${APPNAME}.so lib${APPNAME}.so
    COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/lib${APPNAME}.so ./makecapk/lib/x86_64

    COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/arm32lib${APPNAME}.so lib${APPNAME}.so
    COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/lib${APPNAME}.so ./makecapk/lib/armeabi-v7a

    COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/arm64lib${APPNAME}.so lib${APPNAME}.so
    COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/lib${APPNAME}.so ./makecapk/lib/arm64-v8a

    # As of C++17 (which I use to <filesystem>) it is required to bundle the shared library with processor library
    COMMAND cp ${LLVM_LIBC++}/x86/libc++_shared.so ./makecapk/lib/x86
    COMMAND cp ${LLVM_LIBC++}/x86_64/libc++_shared.so ./makecapk/lib/x86_64
    COMMAND cp ${LLVM_LIBC++}/armeabi-v7a/libc++_shared.so ./makecapk/lib/armeabi-v7a
    COMMAND cp ${LLVM_LIBC++}/arm64-v8a/libc++_shared.so ./makecapk/lib/arm64-v8a
)

add_custom_target( AndroidManifest.xml ALL
    COMMAND @echo "Creating Manifest"
    COMMAND rm -rf AndroidManifest.xml
    COMMAND @echo "Setting up vars"
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/AndroidSpecific/AndroidManifest.xml.template ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND PACKAGENAME=${PACKAGENAME} 
            ANDROIDVERSION=${ANDROIDVERSION} 
            ANDROIDTARGET=${ANDROIDTARGET} 
            APPNAME=${APPNAME} 
            LABEL=${LABEL} 
            envsubst '$$ANDROIDTARGET $$ANDROIDVERSION $$APPNAME $$PACKAGENAME $$LABEL' 
            < AndroidManifest.xml.template 
            > AndroidManifest.xml 
    
    
               
    COMMAND @echo "Finished Manifest"
)

add_custom_command( OUTPUT manifest DEPENDS AndroidManifest.xml)

add_custom_target( makecapk.apk ALL
    DEPENDS
        PROCESSOR_LIBS
        manifest
    COMMAND ${ASSETS} 
    COMMAND mkdir -p makecapk/assets
    COMMAND mkdir -p makecapk/res
    COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/EngineResources/* ${CMAKE_CURRENT_BINARY_DIR}/makecapk/res
    #COMMAND cp -r 
    # Somewhere in the package commands/flags, we require addition of -S [dir] for the asset sources original place
    #[RAWDRAW] We're really cutting corners.  You should probably use resource files.. Replace android:label="@string/app_name" and add a resource file.
    #[RAWDRAW] Then do this -S Sources/res on the aapt line.
    #[RAWDRAW] For icon support, add -S makecapk/res to the aapt line.  also,  android:icon="@mipmap/icon" to your application line in the manifest.
    #[RAWDRAW] If you want to strip out about 800 bytes of data you can remove the icon and strings.

    COMMAND ${AAPT} package -f -F temp.apk -I ${ANDROIDSDK}/platforms/android-${ANDROIDVERSION}/android.jar -M AndroidManifest.xml -S makecapk/res -A makecapk/assets -v --target-sdk-version ${ANDROIDTARGET}
    COMMAND unzip -o temp.apk -d makecapk
    COMMAND rm -rf makecapk.apk
    COMMAND @echo "Now zipping and compressing into apk"
    COMMAND cd makecapk && zip -D9r ../makecapk.apk   .
    COMMAND jarsigner -sigalg SHA1withRSA -digestalg SHA1 -verbose -keystore ${KEYSTOREFILE} -storepass ${STOREPASS} makecapk.apk ${ALIASNAME}
    COMMAND rm -rf ${APKFILE}
    COMMAND ${BUILD_TOOLS}/zipalign -v 4 makecapk.apk ${APKFILE}
    COMMAND rm -rf temp.apk
    COMMAND rm -rf makecapk.apk
    COMMAND @ls -l ${APKFILE}    
)

add_custom_target( uninstall
    COMMAND -${ADB} uninstall ${PACKAGENAME} 
)

add_custom_target(push
    DEPENDS
        makecapk.apk
    COMMAND @echo "Installing " ${PACKAGENAME}
    COMMAND ${ADB} install -r ${APKFILE}
)

add_custom_target(run
    COMMAND push    $(eval ACTIVITYNAME:=$(shell ${AAPT} dump badging ${APKFILE} | grep "launchable-activity" | cut -f 2 -d"'"))
                    ${ADB} shell am start -n ${PACKAGENAME}/${ACTIVITYNAME}
)

add_custom_target( cleanup
    COMMAND -rm -rf temp.apk makecapk.apk makecapk ${APKFILE}
)