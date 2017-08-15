/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2016, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <QApplication>
#include <QSplashScreen>
#include <QThread>
#include <QTimer>
#include "constants.h"
#include "vpiano.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(QSTR_DOMAIN);
    QCoreApplication::setOrganizationDomain(QSTR_DOMAIN);
    QCoreApplication::setApplicationName(QSTR_APPNAME);
    QApplication a(argc, argv);
#if defined(Q_OS_LINUX)
    a.setWindowIcon(QIcon(":/vpiano/vmpk_32x32.png"));
#endif //Q_OS_LINUX
    QPixmap px(":/vpiano/vmpk_splash.png");
    QSplashScreen s(px);
    QFont sf("Arial", 24, QFont::ExtraBold);
    s.setFont(sf);
    s.show();
    s.showMessage("Virtual MIDI Piano Keyboard " + PGM_VERSION, Qt::AlignBottom | Qt::AlignRight);
    QTimer::singleShot(2500, &s, SLOT(close()));
    a.processEvents();
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
