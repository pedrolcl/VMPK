#[===========================================================================[
Virtual MIDI Piano Keyboard
Copyright (C) 2008-2026 Pedro Lopez-Cabanillas <plcl@users.sourceforge.net>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; If not, see <http://www.gnu.org/licenses/>.
#]===========================================================================]

cmake_minimum_required(VERSION 3.16)

project( VMPK
    VERSION 0.9.3
    LANGUAGES CXX
    DESCRIPTION "Virtual MIDI Piano Keyboard"
    HOMEPAGE_URL "https://vmpk.sourceforge.io/"
)

set(DBUS_INIT OFF)
set(FILTER_INIT OFF)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(DBUS_INIT ON)
else()
    set(FILTER_INIT ON)
endif()
option(ENABLE_DBUS "Enable VMPK DBus interface" ${DBUS_INIT})
option(ENABLE_NATIVE_FILTER "Use a QAbstractNativeEventFilter to process raw keyboard events" ${FILTER_INIT})
option(EMBED_TRANSLATIONS "Embed translations instead of installing" OFF)
option(BUILD_DOCS "Build man page" OFF)
option(USE_QT5 "Prefer building with Qt5 instead of Qt6" OFF)
option(INSTALL_DEPENDENCIES "Deploy dependency libraries" OFF)

# set(PROJECT_RELEASE_DATE "September 20, 2026")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_admin")
include(SCMRevision)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# cmake bug https://gitlab.kitware.com/cmake/cmake/issues/18396 closed in cmake 3.14
# if(APPLE AND ${CMAKE_VERSION} VERSION_LESS 3.14)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
# endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra") #-Wzero-as-null-pointer-constant -Wsuggest-override
endif()
#if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
#endif()

# CMAKE_SYSTEM_PROCESSOR is broken on Windows
# cmake bug https://gitlab.kitware.com/cmake/cmake/-/issues/15170 still open today
 if(WIN32)
      string(TOLOWER ${CMAKE_CXX_COMPILER_ARCHITECTURE_ID} CMAKE_SYSTEM_PROCESSOR)
 endif()

if (WIN32)
    # "flat" installation on Windows
    set(CMAKE_INSTALL_BINDIR ".")
    set(CMAKE_INSTALL_LIBDIR ".")
    set(CMAKE_INSTALL_PLUGINS ".")
else()
    include(GNUInstallDirs)
    set(CMAKE_INSTALL_PLUGINS "plugins")
endif()

if (USE_QT5)
    find_package(QT NAMES Qt5 REQUIRED)
    find_package(Qt5 5.9 COMPONENTS Widgets Network LinguistTools REQUIRED)
else()
    find_package(QT NAMES Qt6 REQUIRED)
    find_package(Qt6 6.2 COMPONENTS Gui Widgets Network LinguistTools REQUIRED)
endif()

find_package(Drumstick 2.11 COMPONENTS RT Widgets REQUIRED)

message (STATUS "VMPK v${PROJECT_VERSION}
     install prefix: ${CMAKE_INSTALL_PREFIX}
     Build configuration: ${CMAKE_BUILD_TYPE}
     Operating System: ${CMAKE_SYSTEM_NAME}
     System Processor: ${CMAKE_SYSTEM_PROCESSOR}
     Compiler ID: ${CMAKE_CXX_COMPILER_ID}
     D-Bus support: ${ENABLE_DBUS}
     Native Event Filter: ${ENABLE_NATIVE_FILTER}
     Qt${QT_VERSION_MAJOR} Version: ${QT_VERSION}
     Drumstick Version: ${Drumstick_VERSION}
     CMake Version: ${CMAKE_VERSION}"
)

if (ENABLE_DBUS)
    find_package(Qt${QT_VERSION_MAJOR}DBus REQUIRED)
endif ()

if (UNIX AND NOT APPLE)
    if (ENABLE_NATIVE_FILTER)
        if (LINUX)
            message(WARNING "Native filter does not work on Wayland, it requires X11")
        endif()
        if (QT_VERSION VERSION_LESS 6.0.0)
            find_package(Qt5X11Extras REQUIRED)
        endif()
        find_package(PkgConfig REQUIRED)
        if(PKG_CONFIG_FOUND)
            message(STATUS "Program pkg-config v${PKG_CONFIG_VERSION_STRING} found (${PKG_CONFIG_EXECUTABLE})")
        else()
            message(FATAL_ERROR "Program pkg-config not found")
        endif()
        pkg_check_modules(XCB REQUIRED xcb)
        if(XCB_FOUND)
            message(STATUS "Found XCB development libs v${XCB_VERSION}")
        endif()
    endif()
endif()

add_subdirectory(src)
if (NOT EMBED_TRANSLATIONS)
    add_subdirectory(translations)
endif()

set(vmpk_DATA_FILES
    data/gmgsxg.ins
    data/help.html
    data/help_es.html
    data/help_ru.html
    data/help_sr.html
    data/help_zh_CN.html
    data/azerty.xml
    data/azerty-FR.xml
    data/german.xml
    data/it-qwerty.xml
    data/pc102mac.xml
    data/pc102win.xml
    data/pc102x11.xml
    data/Serbian-cyr.xml
    data/Serbian-lat.xml
    data/spanish.xml
    data/vkeybd-default.xml
    data/txt2ins.awk
)

#unmaintained or deprecated data files:
    #data/hm.html
    #data/hm_es.html
    #data/hm_ru.html
    #data/help_de.html
    #data/help_fr.html
    #data/help_nl.html

if(UNIX)
    add_custom_target (tarball
        COMMAND mkdir -p vmpk-${PROJECT_VERSION}/translations/
        COMMAND cp -r cmake_admin vmpk-${PROJECT_VERSION}
        COMMAND cp -r data vmpk-${PROJECT_VERSION}
        COMMAND cp -r src vmpk-${PROJECT_VERSION}
        COMMAND cp -r man vmpk-${PROJECT_VERSION}
        COMMAND cp -r dbus vmpk-${PROJECT_VERSION}
        COMMAND cp translations/vmpk_{cs,de,es,fr,gl,it,ru,sr,sv,zh_CN}.ts vmpk-${PROJECT_VERSION}/translations/
        COMMAND cp translations/CMakeLists.txt vmpk-${PROJECT_VERSION}/translations/
        COMMAND cp AUTHORS ChangeLog CMakeLists.txt COPYING gpl.rtf NEWS README.md TODO *.nsi *.nsi.in net.sourceforge.VMPK.desktop net.sourceforge.VMPK.metainfo.xml vmpk.pro *.pri vmpk.spec.in qt.conf vmpk-${PROJECT_VERSION}
        COMMAND tar -cj --exclude=.* -f vmpk-${PROJECT_VERSION}.tar.bz2 vmpk-${PROJECT_VERSION}
        COMMAND tar -cz --exclude=.* -f vmpk-${PROJECT_VERSION}.tar.gz vmpk-${PROJECT_VERSION}
        COMMAND zip -qr vmpk-${PROJECT_VERSION}.zip vmpk-${PROJECT_VERSION} -x '.[a-z]*'
        COMMAND rm -rf vmpk-${PROJECT_VERSION}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif ()

if(UNIX AND NOT APPLE)
    configure_file ( vmpk.spec.in
        ${CMAKE_CURRENT_BINARY_DIR}/vmpk.spec
        IMMEDIATE @ONLY )
    add_subdirectory (man)

    install (FILES ${vmpk_DATA_FILES}
             DESTINATION ${CMAKE_INSTALL_DATADIR}/vmpk )

    install (FILES data/vmpk_16x16.png
             DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/16x16/apps
             RENAME vmpk.png)

    install (FILES data/vmpk_32x32.png
             DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps
             RENAME vmpk.png)

    install (FILES data/vmpk_48x48.png
             DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps
             RENAME vmpk.png)

    install (FILES data/vmpk_64x64.png
             DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps
             RENAME vmpk.png)

    install (FILES data/vmpk_128x128.png
             DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps
             RENAME vmpk.png)

    install (FILES data/vmpk_256x256.png
             DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps
             RENAME vmpk.png)

    install (FILES data/vmpk.svgz
             DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps )

    install (FILES net.sourceforge.VMPK.desktop
             DESTINATION ${CMAKE_INSTALL_DATADIR}/applications )

    install (FILES net.sourceforge.VMPK.metainfo.xml
             DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo )

     # uninstall custom target
     configure_file(
         "${CMAKE_SOURCE_DIR}/cmake_admin/cmake_uninstall.cmake.in"
         "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
         IMMEDIATE @ONLY)
     add_custom_target( uninstall
         "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
elseif(WIN32)
    install ( FILES ${vmpk_DATA_FILES}
              DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if (INSTALL_DEPENDENCIES)
    if (CMAKE_SYSTEM MATCHES "Linux")
        find_package(sonivox 4.0 CONFIG REQUIRED shared)
        if (sonivox_FOUND)
            install(IMPORTED_RUNTIME_ARTIFACTS sonivox::sonivox
                    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
        else()
            message(WARNING "Sonivox package not found")
        endif()
        find_package(Drumstick 2.11 CONFIG REQUIRED ALSA)
        if (Drumstick_FOUND)
            install(IMPORTED_RUNTIME_ARTIFACTS Drumstick::ALSA
                    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
        else()
            message(WARNING "Drumstick::ALSA package not found")
        endif()
    endif()
    find_package(FluidSynth CONFIG REQUIRED shared)
    if (FluidSynth_FOUND)
        install(IMPORTED_RUNTIME_ARTIFACTS FluidSynth::libfluidsynth
                RUNTIME_DEPENDENCY_SET fluidsynth-dependencies
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
        if(WIN32)
          set(_pre_exclude api-ms ext-ms wpaxholder hvsi azureattest pdmutilities
                           vcruntime msvcp concrt vccorlib ucrtbase )
          set(_post_exclude [[.*(\\|/)system32(\\|/).*\.dll]])
        elseif(APPLE)
          set(_pre_exclude "/usr/lib" "/System/")
          set(_post_exclude "")
        else()
          set(_pre_exclude "")
          set(_post_exclude ".*/ld-linux-.*" ".*/linux-vdso.*" ".*/libm\\..*" ".*/libc\\..*" ".*/libpthread\\..*" ".*/libdl\\..*" ".*/libstdc\\+\\+\\..*" ".*/libgcc_s.*" ".*/libgomp\\..*" ".*/libQt6.*")
        endif()
        install(RUNTIME_DEPENDENCY_SET fluidsynth-dependencies
                PRE_EXCLUDE_REGEXES ${_pre_exclude}
                POST_EXCLUDE_REGEXES ${_post_exclude}
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    else()
        message(WARNING "Fluidsynth package not found")
    endif()

    if (DEFINED Drumstick_DIR)
        if (Drumstick_DIR MATCHES "cmake")
            set(_drumstick_plugins ${Drumstick_DIR}/../../drumstick2)
        else()
            set(_drumstick_plugins ${Drumstick_DIR}/lib/drumstick2)
        endif()
        if (EXISTS ${_drumstick_plugins})
            install(DIRECTORY ${_drumstick_plugins}
                    DESTINATION ${CMAKE_INSTALL_PLUGINS})
        else()
            message(WARNING "${_drumstick_plugins} does not exists")
        endif()
    else()
        message(WARNING "Drumstick_DIR is not defined")
    endif()

    if (WIN32)
        install(IMPORTED_RUNTIME_ARTIFACTS Drumstick::RT
                LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR})
        install(IMPORTED_RUNTIME_ARTIFACTS Drumstick::Widgets
                LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR})
        set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR})
    else()
        set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endif()

    if (QT_VERSION VERSION_GREATER_EQUAL 6.3)

        set(_deploy_tool_options_arg "")
        if (WIN32)
            set(_deploy_tool_options_arg "
                --plugindir \".\"
                --translationdir \".\"
            ")
        endif()

        if(MSVC)
            qt_generate_deploy_app_script(
                TARGET vmpk
                OUTPUT_SCRIPT deploy_script
                NO_UNSUPPORTED_PLATFORM_ERROR
                NO_COMPILER_RUNTIME
                NO_TRANSLATIONS
                DEPLOY_TOOL_OPTIONS ${_deploy_tool_options_arg}
            )
            include ( InstallRequiredSystemLibraries )
        elseif(WIN32 AND NOT MSVC)
            qt_generate_deploy_app_script(
                TARGET vmpk
                OUTPUT_SCRIPT deploy_script
                NO_UNSUPPORTED_PLATFORM_ERROR
                NO_TRANSLATIONS
            )
        elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
            qt_generate_deploy_app_script(
                TARGET vmpk
                OUTPUT_SCRIPT deploy_script
                INCLUDE_PLUGINS xcb wayland
                NO_UNSUPPORTED_PLATFORM_ERROR
                NO_TRANSLATIONS
            )
        endif()
        if (DEFINED deploy_script)
            install(SCRIPT ${deploy_script})
        endif()
        # AppImage
        if (UNIX AND NOT APPLE)
            install( CODE
              "include(${CMAKE_SOURCE_DIR}/cmake_admin/CreateAppImage.cmake)
               create_appimage(
                    EXE \"vmpk\"
                    ICON \"vmpk.png\"
                    DESKTOP \"net.sourceforge.VMPK.desktop\"
                    WORKDIR \"${CMAKE_CURRENT_BINARY_DIR}\"
                    APPDIR \"vmpk-v${PROJECT_VERSION}-${CMAKE_SYSTEM_PROCESSOR}.AppDir\"
                    ARCH \"${CMAKE_SYSTEM_PROCESSOR}\"
               )"
            )
        endif()
    endif()
endif()

if (DEFINED ENV{GITHUB_OUTPUT})
    file(APPEND
        "$ENV{GITHUB_OUTPUT}"
        "config_version=${PROJECT_VERSION}\n"
    )
endif()

# CPack support
set (CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/gpl.rtf")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Virtual MIDI Piano Keyboard")
set (CPACK_PACKAGE_VENDOR "vmpk.sourceforge.net")
set (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set (CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${CMAKE_PROJECT_VERSION_PATCH})
set (CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}-${PROJECT_VERSION}")
set (CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}" "Virtual MIDI Piano Keyboard")
set (CPACK_PACKAGE_VERSION "v${PROJECT_VERSION}")
set (CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set (CPACK_STRIP_FILES ON)

set (CPACK_SOURCE_IGNORE_FILES "/.svn/;/build/;/share/;~$;.cproject;.project;.user;${CPACK_SOURCE_IGNORE_FILES}")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "vmpk-${PROJECT_VERSION}")
set (CPACK_SOURCE_STRIP_FILES OFF)

# Windows NSIS setup package
if (WIN32)
    set (CPACK_NSIS_DISPLAY_NAME "Virtual MIDI Piano Keyboard")
    set (CPACK_NSIS_WELCOME_TITLE_3LINES ON)
    set (CPACK_NSIS_FINISH_TITLE_3LINES ON)
    set (CPACK_NSIS_HELP_LINK "https://vmpk.sourceforge.io")
    set (CPACK_NSIS_URL_INFO_ABOUT "https://vmpk.sourceforge.io")
    set (CPACK_NSIS_CONTACT "plcl@users.sourceforge.net")
    set (CPACK_NSIS_EXECUTABLES_DIRECTORY ${CMAKE_INSTALL_BINDIR})
    set (CPACK_NSIS_MODIFY_PATH OFF)
endif()

include(CPack)
