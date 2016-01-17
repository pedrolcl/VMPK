# Virtual MIDI Piano Keyboard
# Copyright (C) 2008-2015 Pedro Lopez-Cabanillas <plcl@users.sourceforge.net>
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

cmake_minimum_required(VERSION 2.8.11)
project(VMPK)

set(DBUS_INIT OFF)
set(ENABLE_CORE OFF)
set(ENABLE_WIN OFF)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(DBUS_INIT ON)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(ENABLE_CORE ON)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(ENABLE_WIN ON)
endif()

option(ENABLE_DBUS "Enable VMPK DBus interface" ${DBUS_INIT})

# Show full commands and messages.
# set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL)

# If relative paths break the link scripts, set it OFF
# set(CMAKE_USE_RELATIVE_PATHS OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_definitions(-DRAWKBD_SUPPORT -DPALETTE_SUPPORT)

# Additional CMake modules for 3rd party library checks reside here
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_admin")

# VMPK version string
set (VERSION_MAJOR "0")
set (VERSION_MINOR "6")
set (VERSION_PATCH "2")
set (VERSION_SUFFIX "")
set (VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${VERSION_SUFFIX})
add_definitions (-DVERSION=${VERSION})

message (STATUS "VMPK ${VERSION}
    install prefix: ${CMAKE_INSTALL_PREFIX}
     D-Bus support: ${ENABLE_DBUS}")

# User options
message (STATUS "Build configuration: ${CMAKE_BUILD_TYPE}")

# Check for Qt
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

if (${ENABLE_DBUS})
    find_package(Qt5DBus REQUIRED)
    add_definitions(-DENABLE_DBUS)
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

find_package(PkgConfig REQUIRED)
if(PKG_CONFIG_FOUND)
    message(STATUS "Program pkg-config found (${PKG_CONFIG_EXECUTABLE})")
else()
    message(FATAL_ERROR "Program pkg-config not found")
endif()

if (UNIX AND NOT APPLE)
    find_package(Qt5X11Extras REQUIRED)
# Check for libxcb
    pkg_check_modules(XCB REQUIRED xcb)
    if(XCB_FOUND)
        include_directories(${XCB_INCLUDE_DIRS})
        link_libraries(${XCB_LIBRARIES})
        link_directories(${XCB_LIB_DIR})
    else()
        message(FATAL_ERROR "Please install XCB development libs and headers.")
    endif()
endif()

if (${ENABLE_CORE})
    link_libraries("-framework CoreFoundation -framework Cocoa")
endif ()

if (${ENABLE_WIN})
    #find_package(WINDRES REQUIRED)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
    link_libraries(winmm)
endif ()

pkg_check_modules(DRUMSTICKRT REQUIRED drumstick-rt)
if(DRUMSTICKRT_FOUND)
    link_directories(${DRUMSTICKRT_LIBDIR})
    include_directories(${DRUMSTICKRT_INCLUDEDIR})
    link_libraries(${DRUMSTICKRT_LIBRARIES})
endif()

add_subdirectory(src)
add_subdirectory(translations)
configure_file(vmpk.spec.in vmpk.spec IMMEDIATE @ONLY)

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
    add_custom_target ( distclean
        COMMAND rm -rf build/*
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
#   add_dependencies (distclean clean)

    add_custom_target (tarball
        COMMAND mkdir -p vmpk-${VERSION}/translations/
        COMMAND cp -r cmake_admin vmpk-${VERSION}
        COMMAND cp -r data vmpk-${VERSION}
        COMMAND cp -r src vmpk-${VERSION}
        COMMAND cp -r man vmpk-${VERSION}
        COMMAND cp -r dbus vmpk-${VERSION}
        COMMAND cp translations/vmpk_{cs,de,es,fr,gl,ru,sr,sv}.ts vmpk-${VERSION}/translations/
        COMMAND cp translations/CMakeLists.txt vmpk-${VERSION}/translations/
        COMMAND cp AUTHORS ChangeLog CMakeLists.txt COPYING gpl.rtf NEWS README *.nsi TODO vmpk.desktop vmpk.pro updateqm.pri vmpk.spec.in qt.conf vmpk-${VERSION}
        COMMAND tar -cj --exclude=.* -f vmpk-${VERSION}.tar.bz2 vmpk-${VERSION}
        COMMAND tar -cz --exclude=.* -f vmpk-${VERSION}.tar.gz vmpk-${VERSION}
        COMMAND rm -rf vmpk-${VERSION}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif ()

if(UNIX AND NOT APPLE)

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

    install (FILES vmpk.desktop
             DESTINATION share/applications )

endif ()

if(WIN32)
    include (InstallRequiredSystemLibraries)
	install (FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} 
			 DESTINATION .)
    install (FILES ${vmpk_DATA_FILES}
             DESTINATION .)
endif()

#if(APPLE)
#    set (target "vmpk")
#    configure_file ("${CMAKE_SOURCE_DIR}/cmake_admin/CompleteBundle.cmake.in"
#      "${CMAKE_CURRENT_BINARY_DIR}/build/CompleteBundle.cmake" @ONLY IMMEDIATE)
#    configure_file ("${CMAKE_SOURCE_DIR}/cmake_admin/CreateBundle.sh.in"
#      "${CMAKE_CURRENT_BINARY_DIR}/build/CreateBundle.sh" @ONLY IMMEDIATE)
#    install (SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/build/CompleteBundle.cmake")
#endif ()

# CPack support
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Virtual MIDI Piano Keyboard")
set (CPACK_PACKAGE_VENDOR "vmpk.sourceforge.net")
set (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/gpl.rtf")
set (CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set (CPACK_PACKAGE_INSTALL_DIRECTORY "vmpk")
set (CPACK_PACKAGE_EXECUTABLES "vmpk" "Virtual MIDI Piano Keyboard")

# source packages
set (CPACK_SOURCE_GENERATOR TGZ;TBZ2;ZIP)
set (CPACK_SOURCE_IGNORE_FILES "/.svn/;/build/;/share/;~$;.cproject;.project;.user;${CPACK_SOURCE_IGNORE_FILES}")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "vmpk-${VERSION}")
set (CPACK_SOURCE_STRIP_FILES OFF)

# linux binary packages
if (${CMAKE_SYSTEM} MATCHES "Linux")
    set (CPACK_GENERATOR TGZ;TBZ2)
    set (CPACK_PACKAGE_NAME "vmpk")
    math (EXPR PWORD_SIZE "${CMAKE_SIZEOF_VOID_P}*8")
    set (CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-x86_${PWORD_SIZE})
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

# Mac OSX bundle
#if (APPLE)
#endif ()

include (CPack)
