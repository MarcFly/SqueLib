# TODO: CONVERT THESE INTO INSTALL TARGETS OR WHATEVER ARE THOSE
#------------------------------------------------------------------------------------------------
# MAKING KEYSTORE FILE -> CONVERT INTO A TARGET
#------------------------------------------------------------------------------------------------
macro(make_keystore_file ALIAS_NAME)
    if(ALIAS_NAME STREQUAL "")
        if(NOT ALIASNAME)
            message(STATUS "Setting up alias name")
            set(ALIASNAME standkey)
        endif()
    else()
        set(ALIASNAME ${ALIAS_NAME})
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
endmacro()
#------------------------------------------------------------------------------------------------
# ANDROID INSTALL / PUSH / UNINSTALL / MAKEAPK / ...
#------------------------------------------------------------------------------------------------
macro(squelib_add_targets target)
    if(${CMAKE_BUILD_TYPE} EQUAL "Release")
        set(debuggable "false")
    else()
        set(debuggable "true")
    endif()

    add_custom_target( AndroidManifest.xml
        COMMAND @echo "Creating Manifest"
        COMMAND rm -rf ${CMAKE_BINARY_DIR}/makecapk/AndroidManifest.xml
        COMMAND @echo "Copying Template to Directory"
        COMMAND cp ${SqueLib_defaults}/AndroidSpecific/AndroidManifest.xml.template ${CMAKE_BINARY_DIR}/makecapk
        COMMAND @echo "Substituting Variables with required vals"
        COMMAND PACKAGENAME=${PACKAGENAME} 
                ANDROIDVERSION=${ANDROIDVERSION} 
                ANDROIDTARGET=${ANDROIDTARGET} 
                APPNAME=${target}
                LABEL=${LABEL}
                DEBUGGABLE=${debuggable}
                envsubst '$$ANDROIDTARGET $$ANDROIDVERSION $$APPNAME $$PACKAGENAME $$LABEL $$DEBUGGABLE' 
                < ${CMAKE_BINARY_DIR}/makecapk/AndroidManifest.xml.template 
                > ${CMAKE_BINARY_DIR}/makecapk/AndroidManifest.xml

        COMMAND rm -rf ${CMAKE_BINARY_DIR}/makecapk/AndroidManifest.xml.template
        COMMAND @echo "Finished Manifest"
    )

    #------------------------------------------------------------------------------------------------
    # GENERATE THE INSTALLABLE APK
    #------------------------------------------------------------------------------------------------
    add_custom_target( makecapk.apk
        DEPENDS
            AndroidManifest.xml
        COMMAND ${ASSETS} 
        COMMAND rm -rf ${CMAKE_BINARY_DIR}/temp.apk
        #COMMAND mkdir -p ${CMAKE_BINARY_DIR}/makecapk/assets
        #COMMAND mkdir -p ${CMAKE_BINARY_DIR}/makecapk/res
        
        #COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/EngineResources/* ${CMAKE_BINARY_DIR}/makecapk/res
        #COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/EngineAssets/* ${CMAKE_BINARY_DIR}/makecapk/assets

        COMMAND ${AAPT} package -f -F ${CMAKE_BINARY_DIR}/temp.apk -I ${ANDROIDSDK}/platforms/android-${ANDROIDVERSION}/android.jar -M ${CMAKE_BINARY_DIR}/makecapk/AndroidManifest.xml -S ${CMAKE_BINARY_DIR}/makecapk/res -A ${CMAKE_BINARY_DIR}/makecapk/assets -v --target-sdk-version ${ANDROIDTARGET}
        COMMAND unzip -o ${CMAKE_BINARY_DIR}/temp.apk -d ${CMAKE_BINARY_DIR}/makecapk
        COMMAND rm -rf ${CMAKE_BINARY_DIR}/makecapk.apk
        COMMAND @echo "Now zipping and compressing into apk"
        COMMAND cd ${CMAKE_BINARY_DIR}/makecapk && zip -D9r ${CMAKE_BINARY_DIR}/makecapk.apk .
        COMMAND jarsigner -sigalg SHA1withRSA -digestalg SHA1 -verbose -keystore ${KEYSTOREFILE} -storepass ${STOREPASS} ${CMAKE_BINARY_DIR}/makecapk.apk ${ALIASNAME}
        COMMAND rm -rf ${CMAKE_BINARY_DIR}/${APKFILE}
        COMMAND ${BUILD_TOOLS}/zipalign -v 4 ${CMAKE_BINARY_DIR}/makecapk.apk ${CMAKE_BINARY_DIR}/${APKFILE}
        COMMAND rm -rf ${CMAKE_BINARY_DIR}/temp.apk
        COMMAND rm -rf ${CMAKE_BINARY_DIR}/makecapk.apk
        COMMAND @ls -l ${CMAKE_BINARY_DIR}/${APKFILE}    
    )
    #------------------------------------------------------------------------------------------------
    # UNINSTALL PACKAGE FROM DEVICE IN ADB
    #------------------------------------------------------------------------------------------------
    add_custom_target( uninstall
        COMMAND -${ADB} uninstall ${PACKAGENAME} 
    )
    #------------------------------------------------------------------------------------------------
    # INSTALL THE APK
    #------------------------------------------------------------------------------------------------
    add_custom_target(push
        DEPENDS
            makecapk.apk
        COMMAND @echo "Installing " ${PACKAGENAME}
        COMMAND ${ADB} install -r ${CMAKE_BINARY_DIR}/${APKFILE}
    )
    #------------------------------------------------------------------------------------------------
    # RUN THE APK (NOT WORKING)
    #------------------------------------------------------------------------------------------------
    add_custom_target(run
        COMMAND push    $(eval ACTIVITYNAME:=$(shell ${AAPT} dump badging ${APKFILE} | grep "launchable-activity" | cut -f 2 -d"'"))
                        ${ADB} shell am start -n ${PACKAGENAME}/${ACTIVITYNAME}
    )
    #------------------------------------------------------------------------------------------------
    # FORCEFULLY REMOVE THE GENERATED INTERMEDIATE STEPS, DONE WHILE MAKING APK ALREADY
    #------------------------------------------------------------------------------------------------
    add_custom_target( cleanup
        COMMAND -rm -rf temp.apk makecapk.apk makecapk ${APKFILE}
    )

endmacro()