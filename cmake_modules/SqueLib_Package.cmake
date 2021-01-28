macro(SqueLib_PrepareBuild target OrgName SrcFiles)
    if(ToAndroid)
        set_app_properties(${target} ${OrgName})
        add_library(${target} SHARED "${SrcFiles}")
        link_android_libc(${target} ${NDK} ${OS_NAME} ${ANDROIDVERSION})
    elseif(ToWindows OR ToLinux)
        add_executable(${target} "${SrcFiles}")
    endif()
endmacro()

macro(SqueLib_Package asset_folder resource_folder)
    message(STATUS ${asset_folder} ${resource_folder})
    if(ToAndroid)
        execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/makecapk/assets)
        execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/makecapk/res)
        execute_process(COMMAND cp -r ${asset_folder}/* ${SqueLib_Output})
        execute_process(COMMAND cp -r ${resource_folder}/* ${SqueLib_Output})
    elseif(ToLinux)
        execute_process(COMMAND cp -r ${asset_folder}/* ${SqueLib_Output}/${CMAKE_BUILD_TYPE})
        execute_process(COMMAND cp -r ${resource_folder}/* ${SqueLib_Output}/${CMAKE_BUILD_TYPE})
    elseif(ToWindows)
        execute_process(COMMAND robocopy ${asset_folder} ${SqueLib_Output}/${CMAKE_BUILD_TYPE})
        execute_process(COMMAND robocopy ${resource_folder} ${SqueLib_Output}/${CMAKE_BUILD_TYPE})
    endif()
endmacro()