# Functions

# Get a list of Subdirectories from a Current Directory
function(subdirlist result currdir)
    message(STATUS)
    message(STATUS "subdirlist()")
    file(GLOB children RELATIVE ${currdir} ${currdir}/*)
    set(list "")
    foreach(child ${children})
        if(IS_DIRECTORY ${currdir}/${child})
            message(STATUS "Added ${child} to list")
            LIST(APPEND list ${currdir}/${child})
        endif()
    endforeach()
    set(${result} ${list} PARENT_SCOPE)
    message(STATUS)
endfunction()

# Get the first directory that exists in a list of items
function(first_exists result list)
    message(STATUS)
    message(STATUS "first_exists()")
    message(STATUS "Checking: " "${list}")
    set(found "")
    foreach(item ${list})
        message(STATUS ${item})
        if(IS_DIRECTORY "${item}/")
            message(STATUS "${item} Exists!")
            set(found ${item})
            break()
        endif()
    endforeach()
    message(STATUS "Check ${found}")
    set(${result} ${found} PARENT_SCOPE)
    message(STATUS)
endfunction()

# Macros