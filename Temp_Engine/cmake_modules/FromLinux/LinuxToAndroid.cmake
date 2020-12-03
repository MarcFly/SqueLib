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
set(UNAME $ENV{USER})

set(APP_PLATFORM ANDROIDVERSION)
set(ADB adb)
#=========================================================================
include(${TE_cmake}/FindAndroidSDKVars.cmake)
# Generates NDK, BUILD_TOOLS, ANDROIDSDK variables for directories
#=========================================================================

# Setting compile / linking flags

message(STATUS "Setting Compile Flags for ANDROID")
set(ANDROID_COMPILE_FLAGS "-ffunction-sections -Os -fdata-sections -Wall -fvisibility=hidden ")
set(ANDROID_COMPILE_FLAGS "${ANDROID_COMPILE_FLAGS} -Os -DANDROID -DAPPNAME=${APPNAME} -DANDROID_FULLSCREEN=y -DANDROIDVERSION=${ANDROIDVERSION} ")
set(CFLAGS_ARM64 "-m64 ")
message(STATUS)

message(STATUS "Setting Link Flags for ANDROID")
set(ANDROID_LINK_FLAGS "-Wl --gc-sections -s ") 
set(ANDROID_LINK_FLAGS "${ANDROID_LINK_FLAGS} -shared -uANativeActivity_onCreate ")
message(STATUS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ANDROID_COMPILE_FLAGS} -fPIC ")
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

set(ANDROID_PREBUILT ${NDK}/toolchains/llvm/prebuilt/${OS_NAME})
set(ARM_LIBS ${ANDROID_PREBUILT}/sysroot/usr/lib)
set(ANDROID_PLATFORM aarch64-linux-android)
set(CC_ARM64 ${ANDROID_PREBUILT}/bin/aarch64-linux-android${ANDROIDVERSION}-clang)
set(CCXX_ARM64 ${CC_ARM64}++)

message(STATUS "Setting up compiler")
set(TE_OutputFolder ${CMAKE_BINARY_DIR}/makecapk/lib/arm64-v8a)
message(STATUS "Building fodler for output")
execute_process(COMMAND mkdir -p ${TE_OutputFolder})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ./${TE_OutputFolder})
message(STATUS "Copying to Folder")
execute_process(COMMAND cp ${ARM_LIBS}/${ANDROID_PLATFORM}/libc++_shared.so ${TE_OutputFolder})
set(LLVM_LIBC++ ${CMAKE_CURRENT_BINARY_DIR}/${TE_OutputFolder}/libc++_shared.so)
set(CMAKE_C_COMPILER ${CC_ARM64})
set(CMAKE_CXX_COMPILER ${CCXX_ARM64})
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -v ${CFLAGS_ARM64} ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v ${CFLAGS_ARM64} ")
set(LIBLINK ${PROC_LIBS}/${ANDROID_PLATFORM}/${ANDROIDVERSION})


#set(ANDROID_DIR ${CMAKE_SOURCE_DIR}/EngineCode/android)
#set(app_glue "${NDK}/sources/android/native_app_glue/android_native_app_glue.c")

# NDK App Glue added directly to project to compile
add_library(TempEngine SHARED 
#    ./EngineCode/android/AndroidGlue.cpp # Entry Point and code to interact with android 
#    ${app_glue}  # NDK Native App Glue
    ${ENGINE_SRC}) # Engine Code

set_target_properties(TempEngine
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive"
        LIBRARY_OUTPUT_DIRECTORY "${TE_OutputFolder}"
        RUNTIME_OUTPUT_DIRECTORY "${TE_OutputFolder}")

# Link to necessary NDK Directories
#set(NDKINCLUDE ${NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include)

#target_include_directories(TempEngine PUBLIC 
#    ${NDKINCLUDE} 
#    ${NDKINCLUDE}/android
#    ${NDKINCLUDE}/${INCL_PLATFORM}
#    ${NDK}/sources/android/native_app_glue
#    ${NDK}/sources/android/cpufeatures
#)

#set_target_properties(TempEngine PROPERTIES LINKER_LANGUAGE CXX)

# Hard Linking to the so files makes it work with no issues
# Link flags do not apply properly even when giving the -L directory to link from
# Perform linking to Android shared libraries directly by LIBLINK/lib[libraryname].so

# I don't know why but foreach does not work as intended, it does not create new full
# string on pass, instead adds each item between the full string path

#target_link_libraries(TempEngine PRIVATE 
#    ${LIBLINK}/libm.so
#    ${LIBLINK}/libandroid.so 
#    ${LIBLINK}/libvulkan.so
#    ${LIBLINK}/liblog.so
#    ${LLVM_LIBC++}
#    atomic
#)


#=========================================================================
# Helper Targets to generate the apk
# Requires to call make [target] on the build directory after building the SO files

add_custom_target( AndroidManifest.xml
    COMMAND @echo "Creating Manifest"
    COMMAND rm -rf ${CMAKE_BINARY_DIR}/makecapk/AndroidManifest.xml
    COMMAND @echo "Copying Template to Directory"
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/AndroidSpecific/AndroidManifest.xml.template ${CMAKE_BINARY_DIR}/makecapk
    COMMAND @echo "Substituting Variables with required vals"
    COMMAND PACKAGENAME=${PACKAGENAME} 
            ANDROIDVERSION=${ANDROIDVERSION} 
            ANDROIDTARGET=${ANDROIDTARGET} 
            APPNAME=TempEngine
            LABEL=${LABEL} 
            envsubst '$$ANDROIDTARGET $$ANDROIDVERSION $$APPNAME $$PACKAGENAME $$LABEL' 
            < ${CMAKE_BINARY_DIR}/makecapk/AndroidManifest.xml.template 
            > ${CMAKE_BINARY_DIR}/makecapk/AndroidManifest.xml

    COMMAND rm -rf ${CMAKE_BINARY_DIR}/makecapk/AndroidManifest.xml.template
    COMMAND @echo "Finished Manifest"
)

set(TE_ASSETS_F ${CMAKE_BINARY_DIR}/makecapk/assets)
set(TE_RESOURCE_F ${CMAKE_BINARY_DIR}/makecapk/res)

add_custom_target( makecapk.apk
    DEPENDS
        AndroidManifest.xml
    COMMAND ${ASSETS} 
    COMMAND rm -rf temp.apk
    COMMAND mkdir -p ${TE_ASSETS_F}
    COMMAND mkdir -p ${TE_RESOURCE_F}
    
    COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/EngineResources/* ${CMAKE_BINARY_DIR}/makecapk/res
    COMMAND cp -r ${CMAKE_BINARY_DIR}/makecapk ./
    # HERE COPY ALL Extra libraries built and pulled into TEMP_ENGINE
    #  Example would be ImguiBuilt into a library, not included in code and would have to copy in
    #COMMAND cp -r 
    # Somewhere in the package commands/flags, we require addition of -S [dir] for the asset sources original place
    #[RAWDRAW] We're really cutting corners.  You should probably use resource files.. Replace android:label="@string/app_name" and add a resource file.
    #[RAWDRAW] Then do this -S Sources/res on the aapt line.
    #[RAWDRAW] For icon support, add -S makecapk/res to the aapt line.  also,  android:icon="@mipmap/icon" to your application line in the manifest.
    #[RAWDRAW] If you want to strip out about 800 bytes of data you can remove the icon and strings.

    COMMAND ${AAPT} package -f -F temp.apk -I ${ANDROIDSDK}/platforms/android-${ANDROIDVERSION}/android.jar -M ${CMAKE_BINARY_DIR}/makecapk/AndroidManifest.xml -S ${TE_RESOURCE_F} -A ${TE_ASSETS_F} -v --target-sdk-version ${ANDROIDTARGET}
    COMMAND unzip -o temp.apk -d makecapk
    COMMAND rm -rf makecapk.apk
    COMMAND @echo "Now zipping and compressing into apk"
    COMMAND cd makecapk && zip -D9r ../makecapk.apk .
    COMMAND jarsigner -sigalg SHA1withRSA -digestalg SHA1 -verbose -keystore ${KEYSTOREFILE} -storepass ${STOREPASS} makecapk.apk ${ALIASNAME}
    COMMAND rm -rf ${CMAKE_BINARY_DIR}/${APKFILE}
    COMMAND ${BUILD_TOOLS}/zipalign -v 4 makecapk.apk ${CMAKE_BINARY_DIR}/${APKFILE}
    COMMAND rm -rf temp.apk
    COMMAND rm -rf makecapk.apk
    COMMAND @ls -l ${CMAKE_BINARY_DIR}/${APKFILE}    
)

add_custom_target( uninstall
    COMMAND -${ADB} uninstall ${PACKAGENAME} 
)

add_custom_target(push
    DEPENDS
        makecapk.apk
    COMMAND @echo "Installing " ${PACKAGENAME}
    COMMAND ${ADB} install -r ${CMAKE_BINARY_DIR}/${APKFILE}
)

add_custom_target(run
    COMMAND push    $(eval ACTIVITYNAME:=$(shell ${AAPT} dump badging ${APKFILE} | grep "launchable-activity" | cut -f 2 -d"'"))
                    ${ADB} shell am start -n ${PACKAGENAME}/${ACTIVITYNAME}
)

add_custom_target( cleanup
    COMMAND -rm -rf temp.apk makecapk.apk makecapk ${APKFILE}
)