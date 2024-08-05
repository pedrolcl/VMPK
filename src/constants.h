/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2024, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <QApplication>
#include <QString>

/* Don't translate any string defined in this header */

const QString PGM_VERSION(QT_STRINGIFY(VERSION));

const QString BLD_DATE(__DATE__);
const QString BLD_TIME(__TIME__);
#if defined(Q_CC_CLANG)
const QString CMP_VERSION("CLANG/LLVM " __VERSION__);
#elif defined(Q_CC_GNU) || defined(Q_CC_GCCE)
const QString CMP_VERSION("GNU C++ " __VERSION__);
#elif defined(Q_CC_MSVC)
const QString CMP_VERSION("MSVC " + QString::number(_MSC_VER/100.0f,'f',2));
#else
const QString CMP_VERSION(QString());
#endif

const QString QSTR_APPNAME("VMPK");
const QString QSTR_DOMAIN("vmpk.sourceforge.net");
const QString QSTR_VMPKPX("vmpk");
const QString QSTR_QTPX("qt");
const QString QSTR_DRUMSTICKPX("drumstick-widgets");

const QString QSTR_WINDOW("Window");
const QString QSTR_GEOMETRY("Geometry");
const QString QSTR_STATE("State");
const QString QSTR_PREFERENCES("Preferences");
const QString QSTR_CHANNEL("Channel");
const QString QSTR_VELOCITY("Velocity");
const QString QSTR_BASEOCTAVE("BaseOctave");
const QString QSTR_TRANSPOSE("Transpose");
const QString QSTR_NUMKEYS("NumKeys");
const QString QSTR_STARTINGKEY("StartingKey");
const QString QSTR_INSTRUMENTSDEFINITION("InstrumentsDefinition");
const QString QSTR_INSTRUMENTNAME("InstrumentName");
const QString QSTR_CONNECTIONS("Connections");
const QString QSTR_INENABLED("InEnabled");
const QString QSTR_THRUENABLED("ThruEnabled");
const QString QSTR_OMNIENABLED("OmniEnabled");
const QString QSTR_ADVANCEDENABLED("AdvancedEnabled");
const QString QSTR_INDRIVER("InputDriver");
const QString QSTR_OUTDRIVER("OutputDriver");
const QString QSTR_INPORT("InPort");
const QString QSTR_OUTPORT("OutPort");
const QString QSTR_KEYBOARD("Keyboard");
const QString QSTR_MAPFILE("MapFile");
const QString QSTR_RAWMAPFILE("RawMapFile");
const QString QSTR_DEFAULT("default");
const QString QSTR_CONTROLLERS("Controllers");
const QString QSTR_INSTRUMENT("Instrument");
const QString QSTR_BANK("Bank");
const QString QSTR_PROGRAM("Program");
const QString QSTR_CONTROLLER("Controller");
const QString QSTR_GRABKB("GrabKeyboard");
const QString QSTR_STYLEDKNOBS("StyledKnobs");
const QString QSTR_ALWAYSONTOP("AlwaysOnTop");
const QString QSTR_SHOWSTATUSBAR("ShowStatusBar");
const QString QSTR_RAWKEYBOARDMODE("RawKeyboardMode");
const QString QSTR_EXTRACONTROLLERS("ExtraControllers");
const QString QSTR_EXTRACTLPREFIX("ExtraCtl_");
const QString QSTR_VMPK("VMPK");
const QString QSTR_VMPKINPUT("VMPK Input");
const QString QSTR_VMPKOUTPUT("VMPK Output");
const QString QSTR_DEFAULTINS("gmgsxg.ins");
const QString QSTR_DRUMSCHANNEL("DrumsChannel");
const QString QSTR_SHORTCUTS("Shortcuts");
const QString QSTR_LANGUAGE("Language");
const QString QSTR_VELOCITYCOLOR("VelocityColor");
const QString QSTR_NETWORKPORT("NetworkPort");
const QString QSTR_NETWORKIFACE("NetworkInterface");
const QString QSTR_ENFORCECHANSTATE("EnforceChannelState");
const QString QSTR_ENABLEKEYBOARDINPUT("EnableKeyboardInput");
const QString QSTR_ENABLEMOUSEINPUT("EnableMouseInput");
const QString QSTR_ENABLETOUCHINPUT("EnableTouchInput");

const QString QSTR_MULTICAST_ADDRESS("225.0.0.37");
const QString QSTR_PALETTEPREFIX("Palette_");
const QString QSTR_CURRENTPALETTE("CurrentPalette");
const QString QSTR_SHOWCOLORSCALE("ShowColorScale");
const QString QSTR_CHKBOXSTYLE("QCheckBox::indicator {width: 20px; height: 20px; background: none;} QCheckBox::indicator:checked {image: url(:/vpiano/led_green.png);} QCheckBox::indicator:unchecked{image: url(:/vpiano/led_grey.png);}");
const QString QSTR_DEFAULTFONT("Helvetica, 50");
const QString QSTR_WINSNAP("StickyWindowSnapping");
const QString QSTR_DARKMODE("ForcedDarkMode");
const QString QSTR_STYLE("QtStyle");

#if defined(Q_OS_WINDOWS)
const QString DEFAULTSTYLE("fusion");
#else
const QString DEFAULTSTYLE;
#endif

#if defined(SMALL_SCREEN)
const QString QSTR_VMPKURL("http://vmpk.sourceforge.net/m/");
const QString QSTR_HELP("hm.html");
const QString QSTR_HELPL("hm_%1.html");
#else
const QString QSTR_VMPKURL("http://vmpk.sourceforge.net");
const QString QSTR_HELP("help.html");
const QString QSTR_HELPL("help_%1.html");
#endif

const char MIDICTLNUMBER[] = "MIDICTLNUMBER\0";
const char MIDICTLONVALUE[] = "MIDICTLONVAL\0";
const char MIDICTLOFFVALUE[] = "MIDICTLOFFVAL\0";
const char SYSEXFILENAME[] = "SYSEXFILENAME\0";
const char SYSEXFILEDATA[] = "SYSEXFILEDATA\0";
const char MIDICTLDEFVALUE[] = "MIDICTLDEFVAL\0";

const int MIDIGMDRUMSCHANNEL = 9;
const int MIDICHANNELS = 16;
const int MIDIVELOCITY = 100;
const int MIDIPAN = 64;
const int MIDIVOLUME = 100;
const int MIDIMAXVALUE = 127;
const int DEFAULTBASEOCTAVE = 3;

#if defined(SMALL_SCREEN)
const int DEFAULTNUMBEROFKEYS = 25;
const int TOOLBARLABELMARGIN = 2;
const int KEYLABELFONTSIZE = 3;
#else
const int TOOLBARLABELMARGIN = 5;
#endif
const int NETWORKPORTNUMBER = 21928;

const int RESTART_VMPK = 42;

#endif /*CONSTANTS_H_*/
