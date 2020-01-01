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

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(vmpk_SRCS
    about.ui
    colordialog.ui
    extracontrols.ui
    kmapdialog.ui
    midisetup.ui
    preferences.ui
    riffimportdlg.ui
    vpiano.ui
    shortcutdialog.ui
    fluidsettingsdialog.ui
    networksettingsdialog.ui
    macsynthsettingsdialog.ui
    sonivoxsettingsdialog.ui
    about.cpp
    about.h
    colordialog.cpp
    colordialog.h
    colorwidget.cpp
    colorwidget.h
    constants.h
    extracontrols.cpp
    extracontrols.h
    instrument.cpp
    instrument.h
    keyboardmap.cpp
    keyboardmap.h
    keylabel.cpp
    keylabel.h
    kmapdialog.cpp
    kmapdialog.h
    main.cpp
    mididefs.h
    midisetup.cpp
    midisetup.h
    pianodefs.h
    pianokeybd.cpp
    pianokeybd.h
    pianokey.cpp
    pianokey.h
    pianopalette.cpp
    pianopalette.h
    pianoscene.cpp
    pianoscene.h
    preferences.cpp
    preferences.h
    nativefilter.cpp
    nativefilter.h
    riff.cpp
    riff.h
    riffimportdlg.cpp
    riffimportdlg.h
    shortcutdialog.cpp
    shortcutdialog.h
    fluidsettingsdialog.cpp
    fluidsettingsdialog.h
    networksettingsdialog.cpp
    networksettingsdialog.h
    macsynthsettingsdialog.cpp
    macsynthsettingsdialog.h
    sonivoxsettingsdialog.cpp
    sonivoxsettingsdialog.h
    vpiano.cpp
    vpiano.h
    maceventhelper.h
    ../data/vmpk.qrc
)

if(ENABLE_DBUS)
    add_definitions(-DENABLE_DBUS)
    qt5_add_dbus_adaptor( vmpk_SRCS
                          net.sourceforge.vmpk.xml 
                          vpiano.h 
                          VPiano
                          vmpk_adaptor )
    set_property(SOURCE
        ${CMAKE_CURRENT_BINARY_DIR}/vmpk_adaptor.h
        ${CMAKE_CURRENT_BINARY_DIR}/vmpk_adaptor.cpp
        PROPERTY SKIP_AUTOGEN ON)
endif()

if(WIN32)
    configure_file (vmpk.rc.in ${CMAKE_CURRENT_BINARY_DIR}/vmpk.rc 
                    IMMEDIATE @ONLY)
endif()

if(APPLE)
    set (MACOSX_BUNDLE_INFO_STRING "Virtual MIDI Piano Keyboard" )
    set (MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION} )
    set (MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION} )
    set (MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION} )
    set (MACOSX_BUNDLE_ICON_FILE "vmpk.icns" )
    set (MACOSX_BUNDLE_GUI_IDENTIFIER "net.sourceforge.vmpk" )
    set (MACOSX_BUNDLE_BUNDLE_NAME "vmpk" )
    set (MACOSX_BUNDLE_COPYRIGHT "© 2008-2019 Pedro López-Cabanillas and others")
    set (vmpk_RSC
        ../data/vmpk.icns
        ../data/help.html
        ../data/help_es.html
        ../data/txt2ins.awk
        ../data/gmgsxg.ins
        ../data/spanish.xml
        ../data/german.xml
        ../data/azerty.xml
        ../data/it-qwerty.xml
        ../data/vkeybd-default.xml
        ../data/pc102mac.xml
    )
    set_source_files_properties (${vmpk_RSC} PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources
    )
endif()

add_executable (vmpk MACOSX_BUNDLE WIN32
    $<$<PLATFORM_ID:Darwin>:maceventhelper.mm>
    $<$<PLATFORM_ID:Windows>:${CMAKE_CURRENT_BINARY_DIR}/vmpk.rc>
    ${vmpk_SRCS}
    ${vmpk_RSC}
)

target_link_libraries(vmpk
    Drumstick::RT
    Qt5::Widgets
    Qt5::Network
    $<$<BOOL:${ENABLE_DBUS}>:Qt5::DBus>
    $<$<BOOL:${Qt5X11Extras_FOUND}>:Qt5::X11Extras>
    $<$<BOOL:${XCB_FOUND}>:${XCB_LIBRARIES}>
    $<$<PLATFORM_ID:Windows>:winmm>
)

if (APPLE)
    target_link_libraries(vmpk "-framework CoreFoundation -framework Cocoa")
endif()

target_include_directories(vmpk PUBLIC
    $<$<BOOL:${XCB_FOUND}>:${XCB_INCLUDE_DIRS}>
)

target_compile_definitions(vmpk PUBLIC
    VERSION=${PROJECT_VERSION}
    RAWKBD_SUPPORT
    PALETTE_SUPPORT
    $<$<BOOL:${ENABLE_DBUS}>:ENABLE_DBUS>
)

install (TARGETS vmpk
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR}
)
