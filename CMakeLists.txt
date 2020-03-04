# Virtual MIDI Piano Keyboard
# Copyright (C) 2008-2020 Pedro Lopez-Cabanillas <plcl@users.sourceforge.net>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.9)

project(VMPK
        VERSION 0.7.3.99
        DESCRIPTION "Virtual MIDI Piano Keyboard"
        LANGUAGES CXX
)

set(DBUS_INIT OFF)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(DBUS_INIT ON)
endif()
option(ENABLE_DBUS "Enable VMPK DBus interface" ${DBUS_INIT})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_admin")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# cmake bug https://gitlab.kitware.com/cmake/cmake/issues/18396 closed in cmake 3.14
if(APPLE AND ${CMAKE_VERSION} VERSION_LESS 3.14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

find_package(Qt5 5.12 COMPONENTS Widgets Network LinguistTools REQUIRED)
find_package(Drumstick 2.0 COMPONENTS RT Widgets REQUIRED)

message (STATUS "VMPK v${PROJECT_VERSION}
     install prefix: ${CMAKE_INSTALL_PREFIX}
     Build configuration: ${CMAKE_BUILD_TYPE}
     Processor: ${CMAKE_SYSTEM_PROCESSOR}
     Qt5: ${Qt5_VERSION}
     D-Bus support: ${ENABLE_DBUS}
     Drumstick: ${Drumstick_VERSION}"
)

if (ENABLE_DBUS)
    find_package(Qt5DBus REQUIRED)
endif ()

if (UNIX AND NOT APPLE)
    find_package(Qt5X11Extras REQUIRED)
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

add_subdirectory(src)
add_subdirectory(translations)

# uninstall custom target
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake_admin/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target( uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

set(vmpk_DATA_FILES
    data/gmgsxg.ins
    data/help.html
    data/help_es.html
    data/help_ru.html
    data/help_sr.html
    data/azerty.xml
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
    #data/help_ru.html

if(UNIX)
    add_custom_target (tarball
        COMMAND mkdir -p vmpk-${PROJECT_VERSION}/translations/
        COMMAND cp -r cmake_admin vmpk-${PROJECT_VERSION}
        COMMAND cp -r data vmpk-${PROJECT_VERSION}
        COMMAND cp -r src vmpk-${PROJECT_VERSION}
        COMMAND cp -r man vmpk-${PROJECT_VERSION}
        COMMAND cp -r dbus vmpk-${PROJECT_VERSION}
        COMMAND cp translations/vmpk_{cs,de,es,fr,gl,ru,sr,sv}.ts vmpk-${PROJECT_VERSION}/translations/
        COMMAND cp translations/CMakeLists.txt vmpk-${PROJECT_VERSION}/translations/
        COMMAND cp AUTHORS ChangeLog CMakeLists.txt COPYING gpl.rtf NEWS README *.nsi TODO net.sourceforge.VMPK.desktop net.sourceforge.VMPK.appdata.xml vmpk.pro updateqm.pri vmpk.spec.in qt.conf vmpk-${PROJECT_VERSION}
        COMMAND tar -cj --exclude=.* -f vmpk-${PROJECT_VERSION}.tar.bz2 vmpk-${PROJECT_VERSION}
        COMMAND tar -cz --exclude=.* -f vmpk-${PROJECT_VERSION}.tar.gz vmpk-${PROJECT_VERSION}
        COMMAND rm -rf vmpk-${PROJECT_VERSION}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif ()

if(UNIX AND NOT APPLE)
    configure_file(vmpk.spec.in vmpk.spec IMMEDIATE @ONLY)

    add_subdirectory (man)

    install (FILES ${vmpk_DATA_FILES}
             DESTINATION share/vmpk )

    install (FILES data/vmpk_16x16.png
             DESTINATION share/icons/hicolor/16x16/apps
             RENAME vmpk.png)

    install (FILES data/vmpk_32x32.png
             DESTINATION share/icons/hicolor/32x32/apps
             RENAME vmpk.png)

    install (FILES data/vmpk_48x48.png
             DESTINATION share/icons/hicolor/48x48/apps
             RENAME vmpk.png)

    install (FILES data/vmpk_64x64.png
             DESTINATION share/icons/hicolor/64x64/apps
             RENAME vmpk.png)

    install (FILES data/vmpk_128x128.png
             DESTINATION share/icons/hicolor/128x128/apps
             RENAME vmpk.png)

    install (FILES data/vmpk.svgz
             DESTINATION share/icons/hicolor/scalable/apps )

    install (FILES net.sourceforge.VMPK.desktop
             DESTINATION share/applications )

    install (FILES net.sourceforge.VMPK.appdata.xml
             DESTINATION share/metainfo )
endif ()

if(WIN32)
    include (InstallRequiredSystemLibraries)
	install (FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} 
			 DESTINATION .)
    install (FILES ${vmpk_DATA_FILES}
             DESTINATION .)
endif()

# CPack support
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Virtual MIDI Piano Keyboard")
set (CPACK_PACKAGE_VENDOR "vmpk.sourceforge.net")
set (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
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
