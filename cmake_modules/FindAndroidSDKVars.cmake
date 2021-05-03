macro(setup_android_sdk_vars)

    # Set Route to find SDKs
    set(OS_NAME linux-x86_64)

    if(WIN32)
        set(OS_NAME windows-x86_64)
    endif()

    message(STATUS "Building from " ${OS_NAME})

    # Check that Environment Variable for SDK Exists, else define it
    if(NOT ANDROIDSDK)
        set(SDK_DIRS $ENV{SDK_LOCATIONS} $ENV{ANDROID_HOME} $ENV{ANDROID_SDK_ROOT} ~/Android/Sdk $ENV{HOME}/Android/Sdk c:/dev/AndroidSDK c:/src/AndroidSDK)
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

    if(NOT AAPT)
        set(AAPT ${BUILD_TOOLS}/aapt)
    endif()

    if(NOT ADB)
        if(WSL)
            set(ADB ${ANDROIDSDK}/windows/platform-tools/adb.exe)
        else()
            set(ADB ${ANDROIDSDK}/platform-tools/adb)
        endif()
    endif()
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

endmacro()