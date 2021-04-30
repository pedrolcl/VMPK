find_package(Subversion QUIET)
if (Subversion_FOUND)
    Subversion_WC_INFO(${PROJECT_SOURCE_DIR} PROJECT IGNORE_SVN_FAILURE)
    if (DEFINED PROJECT_WC_REVISION)
        message(STATUS "Current revision (SVN) is ${PROJECT_WC_REVISION}")
    endif()
endif()

if (NOT DEFINED PROJECT_WC_REVISION)
    find_package(Git QUIET)
    if (Git_FOUND)
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            RESULT_VARIABLE res
            OUTPUT_VARIABLE PROJECT_WC_REVISION
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        if (${res} EQUAL 0)
            message(STATUS "Current revision (Git) is ${PROJECT_WC_REVISION}")
        else()
            unset(PROJECT_WC_REVISION)
        endif()
    endif()
endif()

if (DEFINED PROJECT_WC_REVISION)
    set(${PROJECT_NAME}_WC_REVISION ${PROJECT_WC_REVISION})
endif()
