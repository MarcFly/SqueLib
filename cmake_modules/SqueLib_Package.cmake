# Proper Includes, now with macros so they don't auto execute!



macro(SqueLib_PrepareBuild target orgName srcFiles)
    #add_compile_definitions(${CMAKE_BUILD_TYPE}) #Release is a common word, should be names something else... MiniAudio has issue!
    include(${SQUE_cmake_par}/FindAndroidSDKVars.cmake)
    include(${SQUE_cmake_par}/SetupAndroidEnv.cmake)
    include(${SQUE_cmake_par}/AndroidInstallTargets.cmake)

    if(ToAndroid)
        add_compile_definitions(ANDROID)
        add_library(${target} SHARED "${srcFiles}")

        setup_android_sdk_vars()
        make_keystore_file("") # Generating the keystore file takes TOO MUCH TIME
        set_app_properties(${target} ${orgName} 29)
        set_android_link_flags()
        set_android_compile_flags()    
        set_android_compiler(${NDK} ${OS_NAME} ${ANDROIDVERSION})    
        link_android_libc(${target} ${NDK} ${OS_NAME} ${ANDROIDVERSION})
        message(STATUS "Slow 3")
        squelib_add_targets(${target})
        message(STATUS "Slow 4")        
    elseif(ToWindows OR ToLinux)
        message(STATUS "${srcFiles}")
        add_executable(${target} "${srcFiles}")
        if(ToWindows)
            set_target_properties(${target} PROPERTIES
                LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") # Hide Console Always
        endif(ToWindows)
    endif()

    # SOLOUD ----- Has to be included directly to final project... I don't know how from lib and use soloud directly
    set(soloud_base "${SqueLib_external}/soloud")
    file(GLOB_RECURSE soloud_src "${soloud_base}/src/*.cpp" "${soloud_base}/src/*.c" "${soloud_base}/*.h")
    set(soloud_include "${soloud_base}/include")
    target_sources(${target} PUBLIC "${soloud_src}")
    target_include_directories(${target} PUBLIC ${soloud_include})
    source_group(/soloud FILES ${soloud_src})

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        target_compile_options(${target} BEFORE PUBLIC -g)
    endif()
endmacro()

macro(SqueLib_Package asset_folder resource_folder)
    set(own_asset ${asset_folder})
    set(own_resource ${resource_folder})
    message(STATUS ${asset_folder} ${resource_folder} ${SqueLib_defaults})
    if("${resource_folder}" STREQUAL "")
            set(own_resource "${SqueLib_defaults}/Resources")
            
            message(STATUS ${own_resource})
    endif()
    if("${asset_folder}" STREQUAL "")
        set(own_asset "${SqueLib_defaults}/Assets")
        
        message(STATUS ${own_asset})
    endif()
    message(STATUS ${asset_folder} ${resource_folder} ${SqueLib_defaults})
    
    if(ToAndroid)        
        execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/makecapk/assets)
        execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/makecapk/res)
        execute_process(COMMAND cp -a ${own_asset}/. ${CMAKE_BINARY_DIR}/makecapk/assets)
        execute_process(COMMAND cp -a ${own_resource}/. ${CMAKE_BINARY_DIR}/makecapk/res)
    elseif(ToLinux)
        execute_process(COMMAND cp -a ${own_asset}/. ${SqueLib_Output}/Assets)
        execute_process(COMMAND cp -a ${own_resource}/.   ${SqueLib_Output}/Resources)
    elseif(ToWindows)
        execute_process(COMMAND robocopy ${own_asset} ${SqueLib_Output}/${CMAKE_BUILD_TYPE}/Assets /S)
        execute_process(COMMAND robocopy ${own_resource} ${SqueLib_Output}/${CMAKE_BUILD_TYPE}/Resources /S)
        message(STATUS ${CMAKE_BUILD_TYPE})
    endif()
endmacro()