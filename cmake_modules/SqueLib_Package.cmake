# Proper Includes, now with macros so they don't auto execute!



macro(SqueLib_PrepareBuild target orgName srcFiles)
    #add_compile_definitions(${CMAKE_BUILD_TYPE}) #Release is a common word, should be names something else... MiniAudio has issue!
    execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/builds)
    set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/builds)

    include(${SQUE_cmake_par}/SetupAndroidEnv.cmake)
    include(${SQUE_cmake_par}/AndroidInstallTargets.cmake)

    if(ToAndroid)
        add_compile_definitions(ANDROID)
        add_library(${target} SHARED "${srcFiles}")

        #setup_android_sdk_vars()
        make_keystore_file("") # Generating the keystore file takes TOO MUCH TIME
        set_app_properties(${target} ${orgName})
        link_android_libc(${target})
        squelib_add_targets(${target})        
    elseif(ToWindows OR ToLinux)
        message(STATUS "${srcFiles}")
        add_executable(${target} "${srcFiles}")
        if(ToWindows)
            set_target_properties(${target} PROPERTIES
                LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") # Hide Console Always
        endif(ToWindows)
    endif()

    # SOLOUD ----- Has to be included directly to final project... I don't know how from lib and use soloud directly
    if(WITH_SOLOUD)
        target_compile_definitions(${target} PUBLIC WITH_MINIAUDIO)
        set(soloud_base "${SqueLib_extra}/soloud")
        file(GLOB_RECURSE soloud_src "${soloud_base}/src/*.cpp" "${soloud_base}/src/*.c" "${soloud_base}/*.h")
        set(soloud_include "${soloud_base}/include")
        target_sources(${target} PUBLIC "${soloud_src}")
        target_include_directories(${target} PUBLIC ${soloud_include})
        source_group(/soloud FILES ${soloud_src})
    endif()

    if(WITH_GLMATH)
        add_subdirectory("${SqueLib_extra}/glm" ${CMAKE_CURRENT_BINARY_DIR}/GLMath_CMake)
        set(glm_include "${SqueLib_extra}/glm/glm")
        target_include_directories(${target} PUBLIC ${glm_include})
        target_link_libraries(${target} PUBLIC glm)
    endif()

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" AND WITH_MMGR)
        target_compile_options(${target} BEFORE PUBLIC -g)
        set(mmgr_include "${SqueLib_extra}/mmgr")
        set(mmgr_src ${mmgr_include}/mmgr.cpp ${mmgr_include}/mmgr.h ${mmgr_include}/nommgr.h)
        target_sources(${target} PRIVATE "${mmgr_src}")
        target_include_directories(${target} PUBLIC ${mmgr_include})
        source_group(/mmgr FILES ${mmgr_src})
    endif()
    
    set_target_properties(${target}
        PROPERTIES 
            ARCHIVE_OUTPUT_DIRECTORY "${SQUE_OutputFolder}/archive"
            LIBRARY_OUTPUT_DIRECTORY "${SqueLib_Output}"
            RUNTIME_OUTPUT_DIRECTORY "${SqueLib_Output}"
    )
    target_link_libraries(${target} PUBLIC SqueLib)
    target_include_directories(${target} PUBLIC ${SqueLib_include})

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