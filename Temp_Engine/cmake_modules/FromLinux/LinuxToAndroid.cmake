add_compile_definitions(ANDROID)
set(CMAKE_CXX_EXTENSIONS OFF)

# Android App Naming
set(APPNAME '"${PROJ_NAME}"')
set(ORG_NAME "MarcTorresJimenez")
set(LABEL ${PROJ_NAME})
set(APKFILE "${PROJ_NAME}.apk")
set(PACKAGENAME "org.${ORG_NAME}.${PROJ_NAME}")
set(ANDROIDVERSION 29)
set(ANDROIDTARGET ${ANDROIDVERSION})
set(UNAME $ENV{USER})

set(APP_PLATFORM ANDROIDVERSION)
set(ADB adb)
#=========================================================================
include(${FL_cmake}/FindAndroidSDKVars.cmake)
# Generates NDK, BUILD_TOOLS, ANDROIDSDK variables for directories
#=========================================================================

# Setting compile / linking flags

message(STATUS "Setting Compile Flags for ANDROID")
set(ANDROID_COMPILE_FLAGS "-ffunction-sections -Os -fdata-sections -Wall -fvisibility=hidden ")
set(ANDROID_COMPILE_FLAGS "${ANDROID_COMPILE_FLAGS} -Os -DANDROID -DAPPNAME=${APPNAME} -DANDROID_FULLSCREEN=y -DANDROIDVERSION=${ANDROIDVERSION} ")
set(CFLAGS_ARM64 "-m64 ")
set(CFLAGS_ARM32 "-mfloat-abi=softfp -m32 ")
#set(CFLAGS_x86 "-march=i686 -mtune=intel -mssse3 -mfpmath=sse -m32")
#et(CFLAGS_x86_64 "-march=x86-64 -msse4.2 -mpopcnt -m64 -mtune=intel")

message(STATUS)

message(STATUS "Setting Link Flags for ANDROID")
set(ANDROID_LINK_FLAGS "-Wl --gc-sections -s ") 
set(ANDROID_LINK_FLAGS "${ANDROID_LINK_FLAGS} -shared -uANativeActivity_onCreate ")
message(STATUS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ANDROID_COMPILE_FLAGS} -fPIC ")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ANDROID_LINK_FLAGS} ")

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

set(LLVM_LIBC++ ${NDK}/sources/cxx-stl/llvm-libc++/libs)
set(LLVM_LIBC++ ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/sysroot/usr/lib)#Test if libc++ included was the wrong one
set(PROC_LIBS ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/sysroot/usr/lib)

set(CC_ARM64 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/aarch64-linux-android${ANDROIDVERSION}-clang)
set(CCXX_ARM64 ${CC_ARM64}++)
set(CC_ARM32 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/armv7a-linux-androideabi${ANDROIDVERSION}-clang)
set(CCXX_ARM32 ${CC_ARM32}++)

if(ARM64)
    set(Larm64 makecapk/lib/arm64-v8a)
    execute_process(COMMAND mkdir -p ${Larm64})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ./${Larm64})
    execute_process(COMMAND cp ${LLVM_LIBC++}/aarch64-linux-android/libc++_shared.so ./${Larm64})
    set(LLVM_LIBC++ ${CMAKE_BINARY_DIR}/${Larm64}/libc++_shared.so)
    set(CMAKE_C_COMPILER ${CC_ARM64})
    set(CMAKE_CXX_COMPILER ${CCXX_ARM64})
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -v ${CFLAGS_ARM64} ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v ${CFLAGS_ARM64} ")
    set(LIBLINK ${PROC_LIBS}/aarch64-linux-android/${ANDROIDVERSION})
    set(INCL_PLATFORM aarch64-linux-android)
elseif(ARM32)
    set(Larm32 makecapk/lib/armeabi-v7a)
    execute_process(COMMAND mkdir -p ${Larm32})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ./${Larm32})
    execute_process(COMMAND cp ${LLVM_LIBC++}/arm-linux-androideabi/libc++_shared.so ./${Larm32})
    set(LLVM_LIBC++ ${CMAKE_BINARY_DIR}/${Larm32}/libc++_shared.so)
    set(CMAKE_C_COMPILER ${CC_ARM32})
    set(CMAKE_CXX_COMPILER ${CCXX_ARM32})
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -v ${CFLAGS_ARM32} ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v ${CFLAGS_ARM32} ")
    set(LIBLINK ${PROC_LIBS}/arm-linux-androideabi/${ANDROIDVERSION})
    set(INCL_PLATFORM arm-linux-androideabi)
endif()

set(ANDROID_DIR ${CMAKE_SOURCE_DIR}/EngineCode/android)
set(app_glue "${NDK}/sources/android/native_app_glue/android_native_app_glue.c")

# NDK App Glue added directly to project to compile
add_library(${PROJ_NAME} SHARED 
    ./EngineCode/android/AndroidGlue.cpp # Entry Point and code to interact with android 
    ${app_glue}  # NDK Native App Glue
    ${ENGINE_SOURCES}) # Engine Code

# Link to necessary NDK Directories
set(NDKINCLUDE ${NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include)

target_include_directories(${PROJ_NAME} PUBLIC 
    ${NDKINCLUDE} 
    ${NDKINCLUDE}/android
    ${NDKINCLUDE}/${INCL_PLATFORM}
    ${NDK}/sources/android/native_app_glue
    ${NDK}/sources/android/cpufeatures
)

set_target_properties(${PROJ_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Hard Linking to the so files makes it work with no issues
# Link flags do not apply properly even when giving the -L directory to link from
# Perform linking to Android shared libraries directly by LIBLINK/lib[libraryname].so

# I don't know why but foreach does not work as intended, it does not create new full
# string on pass, instead adds each item between the full string path

target_link_libraries(${PROJ_NAME} PUBLIC 
    ${LIBLINK}/libm.so
    ${LIBLINK}/libandroid.so 
    ${LIBLINK}/libvulkan.so
    ${LIBLINK}/liblog.so
    ${LLVM_LIBC++}
    atomic
)


#=========================================================================
# Helper Targets to generate the apk
# Requires to call make [target] on the build directory after building the SO files

add_custom_target( AndroidManifest.xml
    COMMAND @echo "Creating Manifest"
    COMMAND rm -rf AndroidManifest.xml
    COMMAND @echo "Copying Template to Directory"
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/AndroidSpecific/AndroidManifest.xml.template ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND @echo "Substituting Variables with required vals"
    COMMAND PACKAGENAME=${PACKAGENAME} 
            ANDROIDVERSION=${ANDROIDVERSION} 
            ANDROIDTARGET=${ANDROIDTARGET} 
            APPNAME=${PROJ_NAME} 
            LABEL=${LABEL} 
            envsubst '$$ANDROIDTARGET $$ANDROIDVERSION $$APPNAME $$PACKAGENAME $$LABEL' 
            < AndroidManifest.xml.template 
            > AndroidManifest.xml 
    
    
               
    COMMAND @echo "Finished Manifest"
)

add_custom_target( makecapk.apk
    DEPENDS
        AndroidManifest.xml
    COMMAND ${ASSETS} 
    COMMAND rm -rf temp.apk
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
    COMMAND cd makecapk && zip -D9r ../makecapk.apk  .
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