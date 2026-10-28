# Copyright (c) 2026 Pedro López-Cabanillas
# SPDX-License-Identifier: BSD-3-Clause

set(TARGET_ARCH ${CMAKE_HOST_SYSTEM_PROCESSOR}) 
message(STATUS "Running script on: ${CPACK_TEMPORARY_INSTALL_DIRECTORY}")

file(GLOB_RECURSE FRAMEWORK_FILES
     "${CPACK_TEMPORARY_INSTALL_DIRECTORY}/*.app/Contents/Frameworks/*"
)

foreach(BINARY_PATH IN LISTS FRAMEWORK_FILES)
    if(IS_SYMLINK "${BINARY_PATH}" OR IS_DIRECTORY "${BINARY_PATH}" OR "${BINARY_PATH}" MATCHES "\\.(plist|h|modulemap|xcprivacy)$")
        continue()
    endif()
    execute_process(
        COMMAND lipo -thin ${TARGET_ARCH} "${BINARY_PATH}" -output "${BINARY_PATH}.thin"
        RESULT_VARIABLE LIPO_RESULT
        ERROR_VARIABLE LIPO_ERROR
    )
    if(LIPO_RESULT EQUAL 0)
        file(REMOVE "${BINARY_PATH}")
        file(RENAME "${BINARY_PATH}.thin" "${BINARY_PATH}")
    else()
        message(WARNING ${LIPO_ERROR})
    endif()
endforeach()
