#[===========================================================================[
Virtual MIDI Piano Keyboard
Copyright (C) 2008-2024 Pedro Lopez-Cabanillas <plcl@users.sourceforge.net>

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
    VERSION 0.9.1
    LANGUAGES CXX
    DESCRIPTION "Virtual MIDI Piano Keyboard"
    HOMEPAGE_URL "https://vmpk.sourceforge.io/"
)

set(DBUS_INIT OFF)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(DBUS_INIT ON)
endif()
set(FILTER_INIT OFF)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(FILTER_INIT ON)
endif()
option(ENABLE_DBUS "Enable VMPK DBus interface" ${DBUS_INIT})
option(ENABLE_NATIVE_FILTER "Use a QAbstractNativeEventFilter to process raw keyboard events" ${FILTER_INIT})
option(EMBED_TRANSLATIONS "Embed translations instead of installing" OFF)
option(BUILD_DOCS "Build man page" ON)
option(USE_QT5 "Prefer building with Qt5 instead of Qt6")

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

# CMAKE_SYSTEM_PROCESSOR is broken on Windows with MSVC
# cmake bug https://gitlab.kitware.com/cmake/cmake/-/issues/15170 still open in 2024/12
if(MSVC)
    string(TOLOWER ${MSVC_CXX_ARCHITECTURE_ID} CMAKE_SYSTEM_PROCESSOR)
endif()

if (USE_QT5)
    find_package(QT NAMES Qt5 REQUIRED)
    find_package(Qt5 5.9 COMPONENTS Widgets Network LinguistTools REQUIRED)
else()
    find_package(QT NAMES Qt6 REQUIRED)
    find_package(Qt6 6.2 COMPONENTS Gui Widgets Network LinguistTools REQUIRED)
endif()

find_package(Drumstick 2.10 COMPONENTS RT Widgets REQUIRED)

message (STATUS "VMPK v${PROJECT_VERSION}
     install prefix: ${CMAKE_INSTALL_PREFIX}
     Build configuration: ${CMAKE_BUILD_TYPE}
     Operating System: ${CMAKE_SYSTEM_NAME}
     System Processor: ${CMAKE_SYSTEM_PROCESSOR}
     D-Bus support: ${ENABLE_DBUS}
     Native Event Filter: ${ENABLE_NATIVE_FILTER}
     Qt${QT_VERSION_MAJOR} Version: ${QT_VERSION}
     Drumstick Version: ${Drumstick_VERSION}"
)

if (ENABLE_DBUS)
    find_package(Qt${QT_VERSION_MAJOR}DBus REQUIRED)
endif ()

if (UNIX AND NOT APPLE)
    if (ENABLE_NATIVE_FILTER)
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

    include(GNUInstallDirs)
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
else()
    if(WIN32)
        include (InstallRequiredSystemLibraries)
        install (FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
                 DESTINATION .)
        install (FILES ${vmpk_DATA_FILES}
                 DESTINATION .)
        set(FLUIDSYNTH_PREFIX $ENV{FLUIDSYNTH})
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup-msvc2022.nsi.in
            ${CMAKE_CURRENT_BINARY_DIR}/setup-msvc2022.nsi IMMEDIATE @ONLY)
    endif()
endif()

# CPack support
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Virtual MIDI Piano Keyboard")
set (CPACK_PACKAGE_VENDOR "vmpk.sourceforge.net")
set (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/gpl.rtf")
set (CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${CMAKE_PROJECT_VERSION_PATCH})
set (CPACK_PACKAGE_INSTALL_DIRECTORY "vmpk")
set (CPACK_PACKAGE_EXECUTABLES "vmpk" "Virtual MIDI Piano Keyboard")

# source packages
set (CPACK_SOURCE_GENERATOR TGZ;TBZ2;ZIP)
set (CPACK_SOURCE_IGNORE_FILES "/.svn/;/build/;/share/;~$;.cproject;.project;.user;${CPACK_SOURCE_IGNORE_FILES}")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "vmpk-${PROJECT_VERSION}")
set (CPACK_SOURCE_STRIP_FILES OFF)

# linux binary packages
if (${CMAKE_SYSTEM} MATCHES "Linux")
    set (CPACK_GENERATOR TGZ;TBZ2)
    set (CPACK_PACKAGE_NAME "vmpk")
    set (CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
#   set (CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)
#   set (CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
    set (CPACK_STRIP_FILES ON)
endif ()

# Windows NSIS setup package
#if (WIN32)
#    set (CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/src/vmpk.ico")
#    set (CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
#    set (CPACK_NSIS_DISPLAY_NAME "Virtual MIDI Piano Keyboard")
#    set (CPACK_NSIS_HELP_LINK "http://vmpk.sourceforge.net")
#    set (CPACK_NSIS_URL_INFO_ABOUT "http://vmpk.sourceforge.net")
#    set (CPACK_NSIS_CONTACT "plcl@users.sourceforge.net")
#    set (CPACK_NSIS_MODIFY_PATH OFF)
#endif ()

include (CPack)
