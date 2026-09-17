# Copyright (c) 2026 Pedro López-Cabanillas
# SPDX-License-Identifier: BSD-3-Clause
#
# CREATE_APPIMAGE(EXE <exe> ICON <icon-file-name> DESKTOP <desktop-file-name>
#                 WORKDIR <workdir> APPDIR <AppDir-name> ARCH <arch>
#                 [APPIMAGETOOL_PATH <path-to-appimagetool>]
#                 [APPIMAGETOOL_VERSION <version>])
#
# Example:
#   CREATE_APPIMAGE(EXE MyApp ICON myapp.png DESKTOP myapp.desktop
#                   WORKDIR ${CMAKE_BINARY_DIR} APPDIR MyApp.AppDir ARCH x86_64)
#
# Notes:
# - EXE is the executable's basename (will be looked up under <AppDir>/usr/bin/)
# - ICON is the icon filename (expected under usr/share/icons/hicolor/256x256/apps/)
# - DESKTOP is the .desktop filename (expected under usr/share/applications/)
# - If APPIMAGETOOL_PATH is provided, it will be used instead of downloading.
# - This function aims to be robust to spaces in paths and to report clear errors.
# - CMake >= 4.2 has a CPack AppImage generator that may replace this function.

cmake_minimum_required(VERSION 3.16)

function(CREATE_APPIMAGE)
    set(args "EXE;ICON;DESKTOP;WORKDIR;APPDIR;ARCH;APPIMAGETOOL_PATH;APPIMAGETOOL_VERSION")
    cmake_parse_arguments(ARGS "" "${args}" "" ${ARGN})

    # Basic required-argument checks
    foreach(_req EXE ICON DESKTOP WORKDIR APPDIR ARCH)
        if(NOT ARGS_${_req})
            message(FATAL_ERROR "CREATE_APPIMAGE: missing required argument: ${_req}")
        endif()
    endforeach()

    if(ARGS_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "CREATE_APPIMAGE: provided unparsed arguments ${ARGS_UNPARSED_ARGUMENTS}")
    endif()

    # Validate working directory
    if(NOT IS_DIRECTORY "${ARGS_WORKDIR}")
        message(FATAL_ERROR "CREATE_APPIMAGE: invalid working directory: ${ARGS_WORKDIR}")
    endif()

    # Resolve and normalize AppDir path
    get_filename_component(_appdir "${ARGS_WORKDIR}/${ARGS_APPDIR}" ABSOLUTE)
    if(NOT IS_DIRECTORY "${_appdir}")
        message(FATAL_ERROR "CREATE_APPIMAGE: invalid AppDir: ${_appdir}")
    endif()

    # Architecture validation (portable check)
    set(_archs aarch64 armhf i686 x86_64)
    list(FIND _archs "${ARGS_ARCH}" _arch_idx)
    if(_arch_idx EQUAL -1)
        message(FATAL_ERROR "CREATE_APPIMAGE: unsupported architecture: ${ARGS_ARCH}")
    endif()

    # Executable check
    set(_exe "usr/bin/${ARGS_EXE}")
    if(NOT EXISTS "${_appdir}/${_exe}")
        message(FATAL_ERROR "CREATE_APPIMAGE: invalid executable (not found/executable): ${_appdir}/${_exe}")
    endif()

    # Icon check
    set(_icon "usr/share/icons/hicolor/256x256/apps/${ARGS_ICON}")
    if(NOT EXISTS "${_appdir}/${_icon}")
        message(FATAL_ERROR "CREATE_APPIMAGE: icon file not readable: ${_appdir}/${_icon}")
    endif()

    # Desktop file check
    set(_desktop "usr/share/applications/${ARGS_DESKTOP}")
    if(NOT EXISTS "${_appdir}/${_desktop}")
        message(FATAL_ERROR "CREATE_APPIMAGE: .desktop file not readable: ${_appdir}/${_desktop}")
    endif()

    # Determine AppImageTool path (allow user-specified version/path)
    if(ARGS_APPIMAGETOOL_PATH)
        set(_ait_path "${ARGS_APPIMAGETOOL_PATH}")
    else()
        if(ARGS_APPIMAGETOOL_VERSION)
            set(_ait_version "${ARGS_APPIMAGETOOL_VERSION}")
        else()
            set(_ait_version "1.9.1")
        endif()

        set(_ait_path "${ARGS_WORKDIR}/appimagetool-${ARGS_ARCH}.AppImage")

        if(NOT EXISTS "${_ait_path}")
            # Known hashes for appimagetool 1.9.1 (keep these in sync if version changes)
            if("${_ait_version}" STREQUAL "1.9.1")
                if("${ARGS_ARCH}" STREQUAL "aarch64")
                    set(_expected_hash "f0837e7448a0c1e4e650a93bb3e85802546e60654ef287576f46c71c126a9158")
                elseif("${ARGS_ARCH}" STREQUAL "armhf")
                    set(_expected_hash "42b61cba5495d8aaf418a5c9a015a49b85ad92efabcbd3c341f1540440e4e23d")
                elseif("${ARGS_ARCH}" STREQUAL "i686")
                    set(_expected_hash "7ad9ff47c203aae0149b18f6df9e3018b2e2f470ea644a0413e3ded39e9e3bdb")
                elseif("${ARGS_ARCH}" STREQUAL "x86_64")
                    set(_expected_hash "ed4ce84f0d9caff66f50bcca6ff6f35aae54ce8135408b3fa33abfc3cb384eb0")
                endif()
            endif()

            set(_ait_url "https://github.com/AppImage/appimagetool/releases/download/${_ait_version}/appimagetool-${ARGS_ARCH}.AppImage")
            message(STATUS "CREATE_APPIMAGE: downloading AppImageTool from ${_ait_url}")
            if(DEFINED _expected_hash)
                file(DOWNLOAD "${_ait_url}" "${_ait_path}" EXPECTED_HASH SHA256=${_expected_hash} SHOW_PROGRESS)
            else()
                # No expected hash available for this version/arch — download without EXPECTED_HASH
                file(DOWNLOAD "${_ait_url}" "${_ait_path}" SHOW_PROGRESS)
            endif()
            file(CHMOD "${_ait_path}" FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        endif()
    endif()

    if(NOT EXISTS "${_ait_path}")
        message(FATAL_ERROR "CREATE_APPIMAGE: appimagetool not found or not executable: ${_ait_path}")
    endif()

    # AppImage output name - prefer placing output in WORKDIR
    get_filename_component(_appdir_basename "${_appdir}" NAME)
    if(_appdir_basename MATCHES "\\.AppDir$")
        string(REPLACE ".AppDir" ".AppImage" _output_basename "${_appdir_basename}")
    else()
        set(_output_basename "${_appdir_basename}.AppImage")
    endif()
    set(_output_name "${ARGS_WORKDIR}/${_output_basename}")

    # create the symlink that will launch the App (AppRun)
    # Use relative target so the link inside AppDir points to usr/bin/<exe>
    file(REMOVE "${_appdir}/AppRun")
    file(CREATE_LINK "${_exe}" "${_appdir}/AppRun" SYMBOLIC)

    # icon: create a convenient top-level link to the icon basename (this matches older behavior)
    get_filename_component(_icon_basename "${ARGS_ICON}" NAME)
    file(REMOVE "${_appdir}/${_icon_basename}")
    file(CREATE_LINK "${_icon}" "${_appdir}/${_icon_basename}" SYMBOLIC)

    # symlink the .desktop file in the AppDir root
    file(REMOVE "${_appdir}/${ARGS_DESKTOP}")
    file(CREATE_LINK "${_desktop}" "${_appdir}/${ARGS_DESKTOP}" SYMBOLIC)

    # Invoke AppImageTool and capture output
    message(STATUS "CREATE_APPIMAGE: invoking ${_ait_path} to build ${_output_name}")
    execute_process(
        COMMAND "${_ait_path}" -n "${_appdir}" "${_output_name}"
        WORKING_DIRECTORY "${ARGS_WORKDIR}"
        RESULT_VARIABLE _ait_result
        OUTPUT_VARIABLE _ait_stdout
        ERROR_VARIABLE _ait_stderr
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
    )

    if(NOT _ait_result EQUAL 0)
        message(FATAL_ERROR "CREATE_APPIMAGE: appimagetool failed (exit ${_ait_result})\nSTDOUT:\n${_ait_stdout}\nSTDERR:\n${_ait_stderr}")
    else()
        message(STATUS "CREATE_APPIMAGE: created: ${_output_name}")
    endif()
endfunction()
