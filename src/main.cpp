/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2013, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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
*/

#include "constants.h"
#include "vpiano.h"
#if defined(RAWKBD_SUPPORT)
#include "rawkeybdapp.h"
#endif
#include <QApplication>
#if defined(Q_OS_SYMBIAN)
// to lock orientation in Symbian
#include <eikenv.h>
#include <eikappui.h>
#include <aknenv.h>
#include <aknappui.h>
#endif

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(QSTR_DOMAIN);
    QCoreApplication::setOrganizationDomain(QSTR_DOMAIN);
    QCoreApplication::setApplicationName(QSTR_APPNAME);
#if defined(RAWKBD_SUPPORT)
    RawKeybdApp a(argc, argv);
#else
    QApplication a(argc, argv);
#endif
#if defined(Q_OS_LINUX)
    a.setWindowIcon(QIcon(":/vpiano/vmpk_32x32.png"));
#endif //Q_OS_LINUX
#if defined(Q_OS_SYMBIAN)
    // Lock orientation to portrait in Symbian
    CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    TRAP_IGNORE(
        if(appUi) {
            appUi->SetOrientationL(CAknAppUi::EAppUiOrientationLandscape);
        }
    );
#endif //Q_OS_SYMBIAN
    VPiano w;
    if (w.isInitialized()) {
#if defined(SMALL_SCREEN)
        w.showMaximized();
#else
        w.show();
#endif
        return a.exec();
    }
    return EXIT_FAILURE;
}
