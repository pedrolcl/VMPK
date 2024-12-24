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

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (EMBED_TRANSLATIONS)
    set( TRANSLATIONS
         ../translations/vmpk_cs.ts
         ../translations/vmpk_de.ts
         ../translations/vmpk_es.ts
         ../translations/vmpk_fr.ts
         ../translations/vmpk_gl.ts
         ../translations/vmpk_it.ts
         ../translations/vmpk_ru.ts
         ../translations/vmpk_sr.ts
         ../translations/vmpk_sv.ts
         ../translations/vmpk_zh_CN.ts
    )
    if (QT_VERSION VERSION_LESS 5.15.0)
        qt5_add_translation(QM_FILES ${TRANSLATIONS})
    else()
        qt_add_translation(QM_FILES ${TRANSLATIONS})
    endif()
    include(TranslationUtils)
    get_target_property(DRUMSTICK_LOCATION Drumstick::Widgets INTERFACE_INCLUDE_DIRECTORIES)
    get_filename_component(DRUMSTICK_PREFIX ${DRUMSTICK_LOCATION} DIRECTORY)
    file(GLOB WIDGETS_TRANS "${DRUMSTICK_PREFIX}/share/drumstick/drumstick-widgets_*.qm")
    file(COPY ${WIDGETS_TRANS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    add_app_translations_resource(APP_RES ${QM_FILES} ${WIDGETS_TRANS})
    add_qt_translations_resource(QT_RES cs de es fr gl it ru sr sv zh_CN)
endif()

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
    about.cpp
    about.h
    colordialog.cpp
    colordialog.h
    colorwidget.cpp
    colorwidget.h
    constants.h
    extracontrols.cpp
    extracontrols.h
    iconutils.cpp
    iconutils.h
    instrument.cpp
    instrument.h
    keyboardmap.cpp
    keyboardmap.h
    kmapdialog.cpp
    kmapdialog.h
    main.cpp
    mididefs.h
    midisetup.cpp
    midisetup.h
    preferences.cpp
    preferences.h
    riff.cpp
    riff.h
    riffimportdlg.cpp
    riffimportdlg.h
    shortcutdialog.cpp
    shortcutdialog.h
    vpiano.cpp
    vpiano.h
    vpianosettings.cpp
    vpianosettings.h
    ../data/vmpk.qrc
    ${APP_RES}
    ${QT_RES}
)

if (ENABLE_NATIVE_FILTER)
    list(APPEND vmpk_SRCS
        nativefilter.cpp
        nativefilter.h
    )
    if (APPLE)
        list(APPEND vmpk_SRCS
            maceventhelper.mm
            maceventhelper.h
        )
    endif()
endif()

if(WIN32)
    include(CheckCXXSourceCompiles)
    check_cxx_source_compiles("#include <windows.h>
        int main() { EnumWindows([](HWND, LPARAM) -> BOOL { return true; }, 0); return 0; }"
        COMPILER_SUPPORTED)
    if (NOT COMPILER_SUPPORTED)
        message(FATAL_ERROR "Sorry, your compiler is not supported: ${CMAKE_CXX_COMPILER}")
    endif()
    list(APPEND vmpk_SRCS winsnap.cpp winsnap.h)
endif()

if(ENABLE_DBUS)
    if (QT_VERSION VERSION_LESS 5.15.0)
        qt5_add_dbus_adaptor( vmpk_SRCS
                              net.sourceforge.vmpk.xml
                              vpiano.h
                              VPiano
                              vmpk_adaptor )
    else()
        qt_add_dbus_adaptor( vmpk_SRCS
                             net.sourceforge.vmpk.xml
                             vpiano.h
                             VPiano
                             vmpk_adaptor )
    endif()
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
    set (MACOSX_BUNDLE_COPYRIGHT "© 2008-2024 Pedro López-Cabanillas and others")
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
    $<$<PLATFORM_ID:Windows>:${CMAKE_CURRENT_BINARY_DIR}/vmpk.rc>
    ${vmpk_SRCS}
    ${vmpk_RSC}
)

target_link_libraries(vmpk PRIVATE
    Drumstick::RT
    Drumstick::Widgets
    Qt${QT_VERSION_MAJOR}::Widgets
    $<$<BOOL:${ENABLE_DBUS}>:Qt${QT_VERSION_MAJOR}::DBus>
    $<$<BOOL:${Qt5X11Extras_FOUND}>:Qt5::X11Extras>
    $<$<BOOL:${XCB_FOUND}>:${XCB_LIBRARIES}>
)

if (QT_VERSION VERSION_GREATER_EQUAL 6.0.0)
    target_link_libraries(vmpk PRIVATE Qt6::Gui)
endif()

if (WIN32)
    target_link_libraries(vmpk PRIVATE
        winmm
        dwmapi)
endif()

if (APPLE)
    target_link_libraries(vmpk PRIVATE "-framework CoreFoundation -framework Cocoa")
endif()

target_include_directories(vmpk PUBLIC
    $<$<BOOL:${XCB_FOUND}>:${XCB_INCLUDE_DIRS}>
)

target_compile_definitions ( vmpk PUBLIC
    VERSION=${PROJECT_VERSION}
    RAWKBD_SUPPORT
    PALETTE_SUPPORT
    $<$<BOOL:${ENABLE_DBUS}>:ENABLE_DBUS>
    $<$<BOOL:${ENABLE_NATIVE_FILTER}>:ENABLE_NATIVE_FILTER>
)

if (DEFINED PROJECT_WC_REVISION)
    target_compile_definitions ( vmpk PRIVATE
        REVISION=${PROJECT_WC_REVISION} )
endif()

target_compile_definitions(vmpk PRIVATE
    $<$<BOOL:${EMBED_TRANSLATIONS}>:TRANSLATIONS_EMBEDDED>
)

if (UNIX AND NOT APPLE)
    install (TARGETS vmpk
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION ~/Applications
    )
else()
    set(DEPLOYMENT_TARGET vmpk)
    include(DeploymentUtils)
endif()
