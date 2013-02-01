# Virtual MIDI Piano Keyboard
# Copyright (C) 2008-2013 Pedro Lopez-Cabanillas <plcl@users.sourceforge.net>
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

project (VMPK)
cmake_minimum_required (VERSION 2.8)

set (DBUS_INIT OFF)
set (ALSA_INIT OFF)
set (JACK_INIT OFF)
set (CORE_INIT OFF)
set (IRIX_INIT OFF)
set (WIN_INIT OFF)
set (NET_INIT ON)
if (${CMAKE_SYSTEM} MATCHES "Linux")
    set (DBUS_INIT ON)
    set (ALSA_INIT ON)
    set (JACK_INIT ON)
elseif (${CMAKE_SYSTEM} MATCHES "IRIX")
    set (IRIX_INIT ON)
elseif (APPLE)
    set (CORE_INIT ON)
elseif (WIN32)
    set (WIN_INIT ON)
endif ()

option (ENABLE_DBUS "Enable experimental VMPK DBus interface" ${DBUS_INIT})
option (ENABLE_ALSA "Enable ALSA Sequencer driver" ${ALSA_INIT})
option (ENABLE_JACK "Enable Jack MIDI driver" ${JACK_INIT})
option (ENABLE_CORE "Enable Mac OSX CoreMIDI driver" ${CORE_INIT})
option (ENABLE_IRIX "Enable SGI Irix MD driver" ${IRIX_INIT})
option (ENABLE_WIN "Enable Windows MM driver" ${WIN_INIT})
option (ENABLE_NET "Enable UDP Network driver" ${NET_INIT})

# Show full commands and messages.
# set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL)

# If relative paths break the link scripts, set it OFF
# set(CMAKE_USE_RELATIVE_PATHS OFF)

# Assorted settings
set (CMAKE_COLOR_MAKEFILE ON)
set (CMAKE_INCLUDE_CURRENT_DIR TRUE)
set (CMAKE_SKIP_RULE_DEPENDENCY FALSE)
set (CMAKE_CXX_FLAGS_DEBUG "-g -DQT_DEBUG -DDEBUG -D__RTMIDI_DEBUG__")
set (CMAKE_CXX_FLAGS_RELEASE "-O2 -DQT_NO_DEBUG -DNDEBUG")
add_definitions (-DRAWKBD_SUPPORT)

# Additional CMake modules for 3rd party library checks reside here
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_admin")

# VMPK version string
set (VERSION_MAJOR "0")
set (VERSION_MINOR "5")
set (VERSION_PATCH "1")
set (VERSION_SUFFIX "")
set (VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${VERSION_SUFFIX})
add_definitions (-DVERSION=${VERSION})

message (STATUS "VMPK ${VERSION}
    install prefix: ${CMAKE_INSTALL_PREFIX}
     D-Bus support: ${ENABLE_DBUS}
    RtMIDI drivers
              ALSA: ${ENABLE_ALSA}
              JACK: ${ENABLE_JACK}
              Irix: ${ENABLE_IRIX}
          CoreMIDI: ${ENABLE_CORE}
           Windows: ${ENABLE_WIN}
           UDP Net: ${ENABLE_NET}")

# User options
message (STATUS "Build configuration: ${CMAKE_BUILD_TYPE}")

# Check for Qt
set (QT_MIN_VERSION "4.8.0")
find_package (Qt4 REQUIRED)
if (QT4_FOUND)
    message (STATUS "Found Qt4.${QT_EDITION} version = ${QTVERSION}")
else ()
    message (FATAL_ERROR "Qt4 >= 4.8 not found")
endif ()

set (QT_USE_QTXML TRUE)
set (QT_USE_QTSVG TRUE)
if (ENABLE_DBUS)
    set (QT_USE_QTDBUS TRUE)
    add_definitions (-DENABLE_DBUS)
endif ()
if (${ENABLE_NET})
    set (QT_USE_QTNETWORK TRUE)
    add_definitions (-DNETWORK_MIDI)
endif()
include (${QT_USE_FILE})
include_directories (${QT_INCLUDES})
link_directories (${QT_LIBRARY_DIR})
link_libraries (${QT_LIBRARIES})
add_definitions (-DQT_STRICT_ITERATORS)

if (UNIX AND NOT APPLE)
# Check for PKG-CONFIG
    find_package (PkgConfig REQUIRED)
    if (PKG_CONFIG_FOUND)
        message (STATUS "Program pkg-config found (${PKG_CONFIG_EXECUTABLE})")
    else ()
        message (FATAL_ERROR "Program pkg-config not found")
    endif ()
# Check for libX11
    find_package (X11 REQUIRED)
    include_directories (${X11_X11_INCLUDE_PATH})
    link_libraries (${X11_X11_LIB})
endif ()

if (${ENABLE_JACK})
    # Check Jack
    set (HAVE_JACK FALSE)
    PKG_CHECK_MODULES (JACK REQUIRED jack)
    if (JACK_FOUND)
        set (HAVE_JACK TRUE)
        add_definitions (-DHAVE_JACK)
        set (JACK_LIBS ${JACK_LIBRARIES})
        list (APPEND JACK_LIB_DIR ${JACK_LIBRARY_DIRS} ${JACK_LIBDIR})
        list (APPEND JACK_INC_DIR ${JACK_INCLUDE_DIRS} ${JACK_INCLUDEDIR})
        link_directories (${JACK_LIB_DIR})
        include_directories (${JACK_INC_DIR})
        add_definitions (-D__LINUX_JACK__)
        link_libraries (${JACK_LIBS})
    else ()
        message (FATAL_ERROR "Please install Jack development libs and headers.")
    endif ()
endif ()

if (${ENABLE_ALSA})
    find_package (Threads REQUIRED)
    # Check ALSA 1.0
    set (HAVE_ALSA FALSE)
    PKG_CHECK_MODULES (ALSA REQUIRED alsa>=1.0)
    if (ALSA_FOUND)
        set (HAVE_ALSA TRUE)
        add_definitions (-DHAVE_ALSA)
        set (ALSA_LIBS ${ALSA_LIBRARIES})
        list (APPEND ALSA_LIB_DIR ${ALSA_LIBRARY_DIRS} ${ALSA_LIBDIR})
        list (APPEND ALSA_INC_DIR ${ALSA_INCLUDE_DIRS} ${ALSA_INCLUDEDIR})
        link_directories (${ALSA_LIB_DIR})
        include_directories (${ALSA_INC_DIR})
        add_definitions (-D__LINUX_ALSASEQ__)
        add_definitions (-DAVOID_TIMESTAMPING)
        link_libraries (${ALSA_LIBS} ${CMAKE_THREAD_LIBS_INIT})
    else ()
        message(FATAL_ERROR "Please install ALSA development libs and headers.")
    endif ()
endif ()

if (${ENABLE_CORE})
    add_definitions (-D__MACOSX_CORE__)
    link_libraries ("-framework CoreMidi -framework CoreAudio -framework CoreFoundation -framework Carbon")
endif ()

if (${ENABLE_IRIX})
    find_package (Threads REQUIRED)
    add_definitions (-D__IRIX_MD__)
    link_libraries (audio ${CMAKE_THREAD_LIBS_INIT})
endif ()

if (${ENABLE_WIN})
    find_package (WINDRES REQUIRED)
    add_definitions (-D__WINDOWS_MM__)
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
    link_libraries (winmm)
endif ()

add_subdirectory (src)
add_subdirectory (translations)
configure_file (vmpk.spec.in vmpk.spec IMMEDIATE @ONLY)

# uninstall custom target
configure_file (
    "${CMAKE_SOURCE_DIR}/cmake_admin/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target ( uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

set (vmpk_DATA_FILES
    data/azerty.xml
    data/vkeybd-default.xml
    data/it-qwerty.xml
    data/german.xml
    data/gmgsxg.ins
    data/spanish.xml
    data/pc102x11.xml
    data/pc102mac.xml
    data/pc102win.xml
    data/txt2ins.awk
    data/hm.html
    data/hm_es.html
    data/hm_ru.html
    data/help.html
    data/help_de.html
    data/help_es.html
    data/help_fr.html
    data/help_nl.html
    data/help_ru.html
)

if (UNIX)
    add_custom_target ( distclean
        COMMAND rm -rf build/*
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    add_dependencies (distclean clean)

    add_custom_target (tarball
        COMMAND mkdir -p vmpk-${VERSION}/translations/
        COMMAND cp -r cmake_admin vmpk-${VERSION}
        COMMAND cp -r data vmpk-${VERSION}
        COMMAND cp -r src vmpk-${VERSION}
        COMMAND cp -r man vmpk-${VERSION}
        COMMAND cp -r dbus vmpk-${VERSION}
        COMMAND cp translations/vmpk_{cs,de,es,fr,ru,sv}.ts vmpk-${VERSION}/translations/
        COMMAND cp translations/CMakeLists.txt vmpk-${VERSION}/translations/
        COMMAND cp AUTHORS ChangeLog CMakeLists.txt COPYING gpl.rtf NEWS README *.nsi TODO vmpk.desktop vmpk.pro updateqm.pri vmpk.spec.in qt.conf vmpk-${VERSION}
        COMMAND tar -cj --exclude=.* -f vmpk-${VERSION}.tar.bz2 vmpk-${VERSION}
        COMMAND tar -cz --exclude=.* -f vmpk-${VERSION}.tar.gz vmpk-${VERSION}
        COMMAND rm -rf vmpk-${VERSION}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif ()

if (UNIX AND NOT APPLE)

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

if (WIN32)
    include (InstallRequiredSystemLibraries)
    set (QTLIBLIST QtCore QtGui QtXml QtSvg )
    get_filename_component (QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH)
    foreach (qtlib ${QTLIBLIST})
        install (FILES ${QT_DLL_PATH_tmp}/${qtlib}4.dll
                 DESTINATION .)
    endforeach (qtlib)
    if (MINGW)
        install (FILES ${QT_DLL_PATH_tmp}/mingwm10.dll
                       ${QT_DLL_PATH_tmp}/libgcc_s_dw2-1.dll
                 DESTINATION .)
    endif ()
    install (FILES ${vmpk_DATA_FILES}
             DESTINATION .)
endif ()

if (APPLE)
    set (target "vmpk")
    configure_file ("${CMAKE_SOURCE_DIR}/cmake_admin/CompleteBundle.cmake.in"
      "${CMAKE_CURRENT_BINARY_DIR}/build/CompleteBundle.cmake" @ONLY IMMEDIATE)
    configure_file ("${CMAKE_SOURCE_DIR}/cmake_admin/CreateBundle.sh.in"
      "${CMAKE_CURRENT_BINARY_DIR}/build/CreateBundle.sh" @ONLY IMMEDIATE)
    install (SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/build/CompleteBundle.cmake")
endif ()

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
