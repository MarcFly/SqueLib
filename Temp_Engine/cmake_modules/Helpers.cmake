# Functions

# Get a list of Subdirectories from a Current Directory
function(subdirlist result currdir)
    file(GLOB children RELATIVE ${currdir} ${currdir}/*)
    set(${result} "")
    foreach(child ${children})
        if(IS_DIRECTORY ${currdir}/${child})
            LIST(APPEND result ${child})
        endif()
    endforeach()
endfunction()

# Get the first directory that exists in a list of items
function(first_exists result list)
    message(STATUS)
    message(STATUS "first_exists()")
    message(STATUS "Checking: " "${list}")
    foreach(item ${list})
        message(STATUS ${item})
        if(IS_DIRECTORY "${item}/")
            message(STATUS "${item} Exists!")
            set(${result} ${item} PARENT_SCOPE)
            break()
        endif()
    endforeach()
    message(STATUS)
endfunction()

# Macros