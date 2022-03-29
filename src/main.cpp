/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2022, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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
#include <QCommandLineParser>
#include <QSettings>
#include "constants.h"
#include "vpiano.h"
#include "vpianosettings.h"

const char* PGM_DESCRIPTION = QT_TRANSLATE_NOOP("QCoreApplication",
     "Virtual MIDI Piano Keyboard\n"
     "Copyright (C) 2006-2022 Pedro Lopez-Cabanillas\n"
     "This program comes with ABSOLUTELY NO WARRANTY;\n"
     "This is free software, and you are welcome to redistribute it\n"
     "under certain conditions; see the LICENSE for details.");

int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)) &&  (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QSTR_DOMAIN);
    QCoreApplication::setOrganizationDomain(QSTR_DOMAIN);
    QCoreApplication::setApplicationName(QSTR_APPNAME);
    QCoreApplication::setApplicationVersion(PGM_VERSION);
    QCoreApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);
    QCoreApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, false);
#if defined(Q_OS_WINDOWS)
    QApplication::setStyle("fusion");
#elif defined(Q_OS_LINUX)
    app.setWindowIcon(QIcon(":/vpiano/vmpk_32x32.png"));
#endif //Q_OS_LINUX

    VPianoSettings::instance()->loadTranslations();

    QCommandLineParser parser;
    parser.setApplicationDescription(QString("%1 v%2\n\n%3").arg(
        QCoreApplication::applicationName(),
        QCoreApplication::applicationVersion(),
        QCoreApplication::tr(PGM_DESCRIPTION)));
    auto helpOption = parser.addHelpOption();
    auto versionOption = parser.addVersionOption();
    QCommandLineOption portableOption({"p", "portable"}, QCoreApplication::tr("Portable settings mode."));
    QCommandLineOption portableFileOption({"f", "file"}, QCoreApplication::tr("Portable settings file name."), "file");
    parser.addOption(portableOption);
    parser.addOption(portableFileOption);
    parser.process(app);
    if (parser.isSet(versionOption) || parser.isSet(helpOption)) {
        return 0;
    }

    QPixmap px(":/vpiano/vmpk_splash.png");
    QSplashScreen splash(px);
    QFont sf = app.font();
    sf.setPointSize(20);
    splash.setFont(sf);
    splash.show();
    app.processEvents();
    splash.showMessage("Virtual MIDI Piano Keyboard " + PGM_VERSION, Qt::AlignBottom | Qt::AlignRight);
    app.processEvents();

    if (parser.isSet(portableOption) || parser.isSet(portableFileOption)) {
        QString portableFile;
        if (parser.isSet(portableFileOption)) {
            portableFile = parser.value(portableFileOption);
        }
        VPianoSettings::setPortableConfig(portableFile);
    } else {
        QSettings::setDefaultFormat(QSettings::NativeFormat);
    }

    int result_code{EXIT_FAILURE};
    do {
        VPiano w;
        if (w.isInitialized()) {
            splash.finish(&w);
            w.show();
            result_code = app.exec();
        } else {
            splash.close();
        }
    } while (result_code == RESTART_VMPK);
    return result_code;
}
