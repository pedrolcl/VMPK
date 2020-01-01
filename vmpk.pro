# Virtual MIDI Piano Keyboard
# Copyright (C) 2008-2020 Pedro Lopez-Cabanillas <plcl@users.sourceforge.net>
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License along
# with this program; If not, see <http://www.gnu.org/licenses/>.

TEMPLATE = app
TARGET = vmpk
VERSION = 0.7.3
VER_MAJ = 0
VER_MIN = 7
VER_PAT = 3
DEFINES += RAWKBD_SUPPORT PALETTE_SUPPORT

lessThan(QT_MAJOR_VERSION, 5) | lessThan(QT_MINOR_VERSION, 1)  {
    message("Cannot build VMPK with Qt $${QT_VERSION}")
    error("Use Qt 5.1 or newer")
}

QT += core \
    gui \
    widgets \
    network

dbus {
    DEFINES += ENABLE_DBUS
    CONFIG += qdbus
    QT += dbus
    DBUS_ADAPTORS += src/net.sourceforge.vmpk.xml
}

DEFINES += VERSION=$$VERSION

macx {
    INCLUDEPATH += $$(DRUMSTICKINCLUDES)  #$$(HOME)/Library/Frameworks/drumstick-rt.framework/Headers
    LIBS += -F$$(DRUMSTICKLIBS)           #$$(HOME)/Library/Frameworks
    LIBS += -framework drumstick-rt
}

linux* {
    INCLUDEPATH += $$(DRUMSTICKINCLUDES)
    LIBS += -L$$(DRUMSTICKLIBS) -ldrumstick-rt
}

win32 {
    INCLUDEPATH += $$(DRUMSTICKINCLUDES)
    LIBS += -L$$(DRUMSTICKLIBS)
    LIBS += -ldrumstick-rt1
    LIBS += -lws2_32
    RC_FILE = src/vpianoico.rc
}

linux* {
    QT += x11extras
    CONFIG += link_pkgconfig
    PKGCONFIG += xcb
    LIBS += -lpthread
}

macx {
    ICON = data/vmpk.icns
    BUNDLE_RES.files = data/help.html \
        data/help_es.html \
        data/help_sr.html \
        data/help_ru.html \
        data/gmgsxg.ins \
        data/spanish.xml \
        data/german.xml \
        data/azerty.xml \
        data/it-qwerty.xml \
        data/vkeybd-default.xml \
        data/pc102mac.xml \
        data/pc102win.xml \
        data/pc102x11.xml \
        data/Serbian-cyr.xml \
        data/Serbian-lat.xml \
        #qt.conf \
        $$[QT_INSTALL_TRANSLATIONS]/qt_cs.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_de.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_es.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_fr.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_gl.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_ru.qm \
        #$$[QT_INSTALL_TRANSLATIONS]/qt_sr.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_sv.qm \
        vmpk_cs.qm \
        vmpk_de.qm \
        vmpk_es.qm \
        vmpk_fr.qm \
        vmpk_gl.qm \
        vmpk_ru.qm \
        vmpk_sr.qm \
        vmpk_sv.qm
    BUNDLE_RES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += BUNDLE_RES
    QMAKE_TARGET_BUNDLE_PREFIX = net.sourceforge
    QMAKE_BUNDLE = vmpk
    QMAKE_INFO_PLIST = data/Info.plist.app
    LIBS += -framework CoreFoundation \
        -framework Cocoa
}

INCLUDEPATH += src

FORMS += src/about.ui \
    src/colordialog.ui \
    src/extracontrols.ui \
    src/midisetup.ui \
    src/preferences.ui \
    src/riffimportdlg.ui \
    src/vpiano.ui \
    src/networksettingsdialog.ui \
    src/fluidsettingsdialog.ui \
    src/macsynthsettingsdialog.ui \
    src/sonivoxsettingsdialog.ui

HEADERS += src/about.h \
    src/colordialog.h \
    src/colorwidget.h \
    src/constants.h \
    src/extracontrols.h \
    src/instrument.h \
    src/keyboardmap.h \
    src/keylabel.h \
    src/mididefs.h \
    src/midisetup.h \
    src/pianodefs.h \
    src/pianokeybd.h \
    src/pianokey.h \
    src/pianopalette.h \
    src/pianoscene.h \
    src/preferences.h \
    src/nativefilter.h \
    src/riff.h \
    src/riffimportdlg.h \
    src/vpiano.h \
    src/maceventhelper.h \
    src/networksettingsdialog.h \
    src/fluidsettingsdialog.h \
    src/macsynthsettingsdialog.h \
    src/sonivoxsettingsdialog.h

SOURCES += src/about.cpp \
    src/colordialog.cpp \
    src/colorwidget.cpp \
    src/extracontrols.cpp \
    src/instrument.cpp \
    src/keyboardmap.cpp \
    src/keylabel.cpp \
    src/main.cpp \
    src/midisetup.cpp \
    src/pianokeybd.cpp \
    src/pianokey.cpp \
    src/pianopalette.cpp \
    src/pianoscene.cpp \
    src/preferences.cpp \
    src/riff.cpp \
    src/riffimportdlg.cpp \
    src/vpiano.cpp \
    src/networksettingsdialog.cpp \
    src/fluidsettingsdialog.cpp \
    src/macsynthsettingsdialog.cpp \
    src/sonivoxsettingsdialog.cpp

FORMS += src/kmapdialog.ui \
    src/shortcutdialog.ui
HEADERS += src/kmapdialog.h \
    src/shortcutdialog.h
SOURCES += src/kmapdialog.cpp \
    src/shortcutdialog.cpp \
    src/nativefilter.cpp

macx {
    OBJECTIVE_SOURCES += \
        src/maceventhelper.mm
    OTHER_FILES += \
        data/Info.plist.app
}

RESOURCES += data/vmpk.qrc

TRANSLATIONS +=  \
    translations/vmpk_en.ts \
    translations/vmpk_cs.ts \
    translations/vmpk_de.ts \
    translations/vmpk_es.ts \
    translations/vmpk_fr.ts \
    translations/vmpk_gl.ts \
    translations/vmpk_nl.ts \
    translations/vmpk_ru.ts \
    translations/vmpk_sr.ts \
    translations/vmpk_sv.ts \
    translations/vmpk_tr.ts \
    translations/vmpk_zh_CN.ts

### CONFIG += lrelease
include(updateqm.pri)
