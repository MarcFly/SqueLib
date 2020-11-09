add_compile_definitions(ANDROID)

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
set(AAPT ${BUILD_TOOLS}/aapt)

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
set(ANDROID_COMPILE_FLAGS "${ANDROID_COMPILE_FLAGS} -Os -DANDROID -DAPPNAME=${APPNAME} -DANDROID_FULLSCREEN -DANDROIDVERSION=${ANDROIDVERSION}")
set(CFLAGS_ARM64 "-m64")
set(CFLAGS_ARM32 "-mfloat-abi=softfp -m32")
#set(CFLAGS_x86 "-march=i686 -mtune=intel -mssse3 -mfpmath=sse -m32")
#et(CFLAGS_x86_64 "-march=x86-64 -msse4.2 -mpopcnt -m64 -mtune=intel")

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

set(LLVM_LIBC++ ${NDK}/sources/cxx-stl/llvm-libc++/libs)
set(PROC_LIBS ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/sysroot/usr/lib)

set(CC_ARM64 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/aarch64-linux-android${ANDROIDVERSION}-clang)
set(CCXX_ARM64 ${CC_ARM64}++)
set(CC_ARM32 ${NDK}/toolchains/llvm/prebuilt/${OS_NAME}/bin/armv7a-linux-androideabi${ANDROIDVERSION}-clang)
set(CCXX_ARM32 ${CC_ARM32}++)

# It is possible that folder names have to be arm64 and arm32 instead of full name
set(Larm64 makecapk/lib/arm64-v8a) #arm64-v8a
set(Larm32 makecapk/lib/armeabi-v7a) #armeabi-v7a
if(ARM64)
    execute_process(COMMAND mkdir -p ${Larm64})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ./${Larm64})
    execute_process(COMMAND cp ${LLVM_LIBC++}/arm64-v8a/libc++_shared.so ./${Larm64})
    set(CMAKE_C_COMPILER ${CC_ARM64})
    set(CMAKE_CXX_COMPILER ${CCXX_ARM64})
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -v ${CFLAGS_ARM64}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v ${CFLAGS_ARM64}")
    set(CMAKE_EXE_LINKER_FLAGS "-L${PROC_LIBS}/aarch64-linux-android/${ANDROIDVERSION} ${CMAKE_EXE_LINKER_FLAGS}")

elseif(ARM32)
    execute_process(COMMAND mkdir -p ${Larm32})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ./${Larm32})
    execute_process(COMMAND cp ${LLVM_LIBC++}/armeabi-v7a/libc++_shared.so ./${Larm32})
    set(CMAKE_C_COMPILER ${CC_ARM32})
    set(CMAKE_CXX_COMPILER ${CCXX_ARM32})
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -v ${CFLAGS_ARM32}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v ${CFLAGS_ARM32}")
    set(CMAKE_EXE_LINKER_FLAGS "-L${PROC_LIBS}/arm-linux-androideabi/${ANDROIDVERSION} ${CMAKE_EXE_LINKER_FLAGS}")
endif()

add_library(app_glue STATIC "${NDK}/sources/android/native_app_glue/android_native_app_glue.c")
target_include_directories(app_glue PUBLIC ${NDK}/sources/android/native_app_glue)
set_target_properties(app_glue PROPERTIES LINKER_LANGUAGE C)

add_library(${PROJ_NAME} SHARED ./EngineCode/test.cpp) #"${ENGINE_SOURCES}")
target_include_directories(${PROJ_NAME} PUBLIC ${NDK}/sysroot/usr/include/android)
target_include_directories(${PROJ_NAME} PUBLIC ${NDK}/sources/android/native_app_glue)

target_link_libraries(${PROJ_NAME} PUBLIC app_glue)

set_target_properties(${PROJ_NAME} PROPERTIES LINKER_LANGUAGE CXX)

#=========================================================================
# Helper Targets to generate the apk
# Requires to call make [target] on the uild directory after building the SO files

add_custom_target( AndroidManifest.xml
    COMMAND @echo "Creating Manifest"
    COMMAND rm -rf AndroidManifest.xml
    COMMAND @echo "Setting up vars"
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/AndroidSpecific/AndroidManifest.xml.template ${CMAKE_CURRENT_BINARY_DIR}
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