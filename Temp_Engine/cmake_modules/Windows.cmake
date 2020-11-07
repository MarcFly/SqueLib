#Add an executable entry point
add_executable(${PROJ_NAME} ${MAIN})

#Target Sources for Own Engine things
target_sources(${PROJ_NAME} PUBLIC ${HELPERS} ${MODULES} ${APP_SOURCE})

#Make Folders inside IDEs
source_group("helpers" FILES ${HELPERS})
source_group("application" FILES ${APP_SOURCE})
source_group("modules" FILES ${MODULES})

#Link Libraries to target executable