macro(SqueLib_PrepareBuild target OrgName SrcFiles)
    if(ToAndroid)
        add_compile_definitions(ANDROID)
        add_library(${target} SHARED "${SrcFiles}")

        include(${SQUE_cmake_par}/FindAndroidSDKVars.cmake)
        include(${SQUE_cmake_par}/SetupAndroidEnv.cmake)
        set_app_properties(${target} ${OrgName})
        set_android_link_flags()
        set_android_compile_flags()    
        set_android_compiler(${NDK} ${OS_NAME} ${ANDROIDVERSION})    
        link_android_libc(${target} ${NDK} ${OS_NAME} ${ANDROIDVERSION})
        message(STATUS "Slow 3")
        include(${SQUE_cmake_par}/AndroidInstallTargets.cmake)
        squelib_add_targets(${target})
        message(STATUS "Slow 4")        
    elseif(ToWindows OR ToLinux)
        add_executable(${target} "${SrcFiles}")
        if(ToWindows)
            set_target_properties(${target} PROPERTIES
                LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") # Hide Console Always
        endif(ToWindows)
    endif()
endmacro()

macro(SqueLib_Package asset_folder resource_folder)
    message(STATUS ${asset_folder} ${resource_folder})
    if(ToAndroid)
        execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/makecapk/assets)
        execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/makecapk/res)
        execute_process(COMMAND cp -a ${asset_folder}/. ${CMAKE_BINARY_DIR}/makecapk/assets)
        execute_process(COMMAND cp -a ${resource_folder}/. ${CMAKE_BINARY_DIR}/makecapk/res)
    elseif(ToLinux)
        execute_process(COMMAND cp -a ${asset_folder}/. ${SqueLib_Output})
        execute_process(COMMAND cp -a ${resource_folder}/.   ${SqueLib_Output})
    elseif(ToWindows)
        execute_process(COMMAND robocopy ${asset_folder} ${SqueLib_Output}/${CMAKE_BUILD_TYPE})
        execute_process(COMMAND robocopy ${resource_folder} ${SqueLib_Output}/${CMAKE_BUILD_TYPE})
    endif()
endmacro()