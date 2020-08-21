/*
 MIDI Virtual Piano Keyboard
 Copyright (C) 2008-2020, Pedro Lopez-Cabanillas <plcl@users.sf.net>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QTouchDevice>
#include <drumstick/settingsfactory.h>
#include <drumstick/backendmanager.h>
#include <drumstick/pianokeybd.h>
#include "vpianosettings.h"
#include "constants.h"

using namespace drumstick::rt;
using namespace drumstick::widgets;

VPianoSettings::VPianoSettings(QObject *parent) : QObject(parent)
{
    m_defaultInputBackend = QLatin1String("Network");
    m_defaultInputConnection = QLatin1String("21928");
#if defined(Q_OS_LINUX)
    m_defaultOutputBackend = QLatin1String("SonivoxEAS");
    m_defaultOutputConnection = QLatin1String("SonivoxEAS");
#elif defined(Q_OS_MACOS)
    m_defaultOutputBackend = QLatin1String("DLS Synth");
    m_defaultOutputConnection = QLatin1String("DLS Synth");
#elif defined(Q_OS_WIN)
    m_defaultOutputBackend = QLatin1String("Windows MM");
    m_defaultOutputConnection = QLatin1String("Microsoft GS Wavetable Synth");
#else
    m_defaultOutputBackend = m_defaultInputBackend;
    m_defaultOutputConnection = m_defaultInputConnection;
#endif
    ResetDefaults();
    initializePalettes();
}

VPianoSettings* VPianoSettings::instance()
{
    static VPianoSettings inst;
    return &inst;
}

void VPianoSettings::ResetDefaults()
{
    m_midiThru = false;
    m_advanced = false;
    m_inputEnabled = true;
    m_omniMode = false;
    m_baseChannel = 0;
    m_velocity = 100;
    m_baseOctave = 1;
    //m_numKeys = 88;
    m_startingKey = 9;
    m_defaultsMap = QVariantMap{
        { drumstick::rt::QSTR_DRUMSTICKRT_PUBLICNAMEIN, QSTR_VMPKINPUT},
        { drumstick::rt::QSTR_DRUMSTICKRT_PUBLICNAMEOUT, QSTR_VMPKOUTPUT}
    };

    m_paletteId = drumstick::widgets::PAL_SINGLE;
    m_numKeys = DEFAULTNUMBEROFKEYS;
    m_drumsChannel = MIDIGMDRUMSCHANNEL;
    m_alwaysOnTop = false;
    m_rawKeyboard = false;
    m_velocityColor = true;
    m_enforceChannelState = false;
    m_enableKeyboard = true;
    m_enableMouse = true;
    m_enableTouch = true;

    emit ValuesChanged();
}

void VPianoSettings::ReadSettings()
{
    SettingsFactory settings;
    internalRead(*settings);
}

void VPianoSettings::SaveSettings()
{
    SettingsFactory settings;
    internalSave(*settings);
}

void VPianoSettings::ReadFromFile(const QString &filepath)
{
    QSettings settings(filepath, QSettings::IniFormat);
    internalRead(settings);
}

void VPianoSettings::SaveToFile(const QString &filepath)
{
    QSettings settings(filepath, QSettings::IniFormat);
    internalSave(settings);
}

void VPianoSettings::internalRead(QSettings &settings)
{
    settings.beginGroup(QSTR_WINDOW);
    m_geometry = settings.value(QSTR_GEOMETRY).toByteArray();
    m_state = settings.value(QSTR_STATE).toByteArray();
    settings.endGroup();

    settings.beginGroup(QSTR_DRUMSTICKRT_GROUP);
    QStringList keys = settings.allKeys();
    for(const QString& key : m_defaultsMap.keys()) {
        if (!keys.contains(key)) {
            keys.append(key);
        }
    }
    for(const QString& key : keys) {
        QVariant defval = m_defaultsMap.contains(key) ? m_defaultsMap[key] : QString();
        m_settingsMap.insert(key, settings.value(key, defval));
    }
    settings.endGroup();

    settings.beginGroup(QSTR_CONNECTIONS);
    m_inputEnabled = settings.value(QSTR_INENABLED, true).toBool();
    m_midiThru = settings.value(QSTR_THRUENABLED, true).toBool();
    m_omniMode = settings.value(QSTR_OMNIENABLED, false).toBool();
    m_advanced = settings.value(QSTR_ADVANCEDENABLED, false).toBool();
    m_lastInputBackend = settings.value(QSTR_INDRIVER, m_defaultInputBackend).toString();
    m_lastOutputBackend = settings.value(QSTR_OUTDRIVER, m_defaultOutputBackend).toString();
    m_lastInputConnection = settings.value(QSTR_INPORT, m_defaultInputConnection).toString();
    m_lastOutputConnection = settings.value(QSTR_OUTPORT, m_defaultOutputConnection).toString();
    settings.endGroup();

    bool mouseInputEnabledbyDefault = true;
    bool touchInputEnabledbyDefault = false;
    for(const QTouchDevice *dev : QTouchDevice::devices()) {
        if (dev->type() == QTouchDevice::TouchScreen) {
            mouseInputEnabledbyDefault = false;
            touchInputEnabledbyDefault = true;
            break;
        }
    }

    settings.beginGroup(QSTR_PREFERENCES);
    m_baseChannel = settings.value(QSTR_CHANNEL, 0).toInt();
    m_velocity = settings.value(QSTR_VELOCITY, MIDIVELOCITY).toInt();
    m_baseOctave = settings.value(QSTR_BASEOCTAVE, 3).toInt();
    m_transpose = settings.value(QSTR_TRANSPOSE, 0).toInt();
    m_numKeys = settings.value(QSTR_NUMKEYS, DEFAULTNUMBEROFKEYS).toInt();
    m_insFileName = settings.value(QSTR_INSTRUMENTSDEFINITION).toString();
    m_insName = settings.value(QSTR_INSTRUMENTNAME).toString();
    m_alwaysOnTop = settings.value(QSTR_ALWAYSONTOP, false).toBool();
    m_showNames = settings.value(QSTR_SHOWNOTENAMES, false).toBool();
    m_showStatusBar = settings.value(QSTR_SHOWSTATUSBAR, false).toBool();
    m_velocityColor = settings.value(QSTR_VELOCITYCOLOR, true).toBool();
    m_enforceChannelState = settings.value(QSTR_ENFORCECHANSTATE, false).toBool();
    m_enableKeyboard = settings.value(QSTR_ENABLEKEYBOARDINPUT, true).toBool();
    m_enableMouse = settings.value(QSTR_ENABLEMOUSEINPUT, mouseInputEnabledbyDefault).toBool();
    m_enableTouch = settings.value(QSTR_ENABLETOUCHINPUT, touchInputEnabledbyDefault).toBool();
    m_drumsChannel = settings.value(QSTR_DRUMSCHANNEL, MIDIGMDRUMSCHANNEL).toInt();
    m_startingKey = settings.value(QSTR_STARTINGKEY, DEFAULTSTARTINGKEY).toInt();
    m_paletteId = settings.value(QSTR_CURRENTPALETTE, PAL_SINGLE).toInt();
    m_colorScale = settings.value(QSTR_SHOWCOLORSCALE, false).toBool();
    m_language = settings.value(QSTR_LANGUAGE, QLocale::system().name()).toString();
    settings.endGroup();

    setCurrentPalette(m_paletteId);
    currentPalette()->loadColors();

    settings.beginGroup(QSTR_KEYBOARD);
    m_rawKeyboard = settings.value(QSTR_RAWKEYBOARDMODE, false).toBool();
    m_mapFile = settings.value(QSTR_MAPFILE, QSTR_DEFAULT).toString();
    m_rawMapFile = settings.value(QSTR_RAWMAPFILE, QSTR_DEFAULT).toString();
    settings.endGroup();

    settings.beginGroup(QSTR_SHORTCUTS);
    for(const QString& sKey : settings.childKeys()) {
        const QString& sValue = settings.value('/' + sKey).toString();
        m_shortcuts.insert(sKey, sValue);
    }
    settings.endGroup();

    m_keymap.copyFrom(&g_DefaultKeyMap, false);
    m_rawmap.copyFrom(&g_DefaultRawKeyMap, true);

    emit ValuesChanged();
}

void VPianoSettings::internalSave(QSettings &settings)
{
    settings.beginGroup(QSTR_WINDOW);
    settings.setValue("Geometry", m_geometry);
    settings.setValue("State", m_state);
    settings.endGroup();

    settings.beginGroup(QSTR_DRUMSTICKRT_GROUP);
    for(auto key : m_settingsMap.keys()) {
        settings.setValue(key, m_settingsMap[key]);
    }
    settings.endGroup();

    settings.beginGroup(QSTR_CONNECTIONS);
    settings.setValue(QSTR_INENABLED, m_inputEnabled);
    settings.setValue(QSTR_THRUENABLED, m_midiThru);
    settings.setValue(QSTR_OMNIENABLED, m_omniMode);
    settings.setValue(QSTR_ADVANCEDENABLED, m_advanced);
    settings.setValue(QSTR_INDRIVER, m_lastInputBackend);
    settings.setValue(QSTR_OUTDRIVER, m_lastOutputBackend);
    settings.setValue(QSTR_INPORT, m_lastInputConnection);
    settings.setValue(QSTR_OUTPORT, m_lastOutputConnection);
    settings.endGroup();

    settings.beginGroup(QSTR_PREFERENCES);
    settings.setValue(QSTR_CHANNEL, m_baseChannel);
    settings.setValue(QSTR_VELOCITY, m_velocity);
    settings.setValue(QSTR_BASEOCTAVE, m_baseOctave);
    settings.setValue(QSTR_TRANSPOSE, m_transpose);
    settings.setValue(QSTR_NUMKEYS, m_numKeys);
    settings.setValue(QSTR_INSTRUMENTSDEFINITION, m_insFileName);
    settings.setValue(QSTR_INSTRUMENTNAME, m_insName);
    settings.setValue(QSTR_ALWAYSONTOP, m_alwaysOnTop);
    settings.setValue(QSTR_SHOWNOTENAMES, m_showNames);
    settings.setValue(QSTR_SHOWSTATUSBAR, m_showStatusBar);
    settings.setValue(QSTR_VELOCITYCOLOR, m_velocityColor);
    settings.setValue(QSTR_ENFORCECHANSTATE, m_enforceChannelState);
    settings.setValue(QSTR_ENABLEKEYBOARDINPUT, m_enableKeyboard);
    settings.setValue(QSTR_ENABLEMOUSEINPUT, m_enableMouse);
    settings.setValue(QSTR_ENABLETOUCHINPUT, m_enableTouch);
    settings.setValue(QSTR_DRUMSCHANNEL, m_drumsChannel);
    settings.setValue(QSTR_STARTINGKEY, m_startingKey);
    settings.setValue(QSTR_CURRENTPALETTE, m_paletteId);
    settings.setValue(QSTR_SHOWCOLORSCALE, m_colorScale);
    settings.endGroup();

    settings.beginGroup(QSTR_KEYBOARD);
    settings.setValue(QSTR_RAWKEYBOARDMODE, m_rawKeyboard);
    settings.setValue(QSTR_MAPFILE, m_mapFile);
    settings.setValue(QSTR_RAWMAPFILE, m_rawMapFile);
    settings.endGroup();

    settings.sync();

    saveCurrentPalette();
}

QString VPianoSettings::getRawMapFile() const
{
    return m_rawMapFile;
}

void VPianoSettings::setRawMapFile(const QString &rawMapFile)
{
    QFileInfo f(rawMapFile);
    if (f.isReadable()) {
        m_rawmap.loadFromXMLFile(rawMapFile);
        m_rawMapFile = rawMapFile;
    } else {
        m_rawmap.clear();
        m_rawmap.setFileName(QSTR_DEFAULT);
        m_rawMapFile = QSTR_DEFAULT;
    }
}

QString VPianoSettings::getMapFile() const
{
    return m_mapFile;
}

void VPianoSettings::setMapFile(const QString &mapFile)
{
    QFileInfo f(mapFile);
    if (f.isReadable()) {
        m_keymap.loadFromXMLFile(mapFile);
        m_mapFile = mapFile;
    } else {
        m_keymap.clear();
        m_keymap.setFileName(QSTR_DEFAULT);
        m_mapFile = QSTR_DEFAULT;
    }
}

bool VPianoSettings::colorScale() const
{
    return m_colorScale;
}

void VPianoSettings::setColorScale(bool colorScale)
{
    m_colorScale = colorScale;
}

bool VPianoSettings::showStatusBar() const
{
    return m_showStatusBar;
}

void VPianoSettings::setShowStatusBar(bool showStatusBar)
{
    m_showStatusBar = showStatusBar;
}

bool VPianoSettings::showNames() const
{
    return m_showNames;
}

void VPianoSettings::setShowNames(bool showNames)
{
    m_showNames = showNames;
}

QString VPianoSettings::defaultOutputConnection() const
{
    return m_defaultOutputConnection;
}

QString VPianoSettings::defaultInputConnection() const
{
    return m_defaultInputConnection;
}

QString VPianoSettings::insName() const
{
    return m_insName;
}

void VPianoSettings::setInsName(const QString &insName)
{
    m_insName = insName;
}

int VPianoSettings::transpose() const
{
    return m_transpose;
}

void VPianoSettings::setTranspose(int transpose)
{
    m_transpose = transpose;
}

int VPianoSettings::paletteId() const
{
    return m_paletteId;
}

void VPianoSettings::setPaletteId(int paletteId)
{
    m_paletteId = paletteId;
}

QString VPianoSettings::insFileName() const
{
    return m_insFileName;
}

void VPianoSettings::setInsFileName(const QString &insFileName)
{
    m_insFileName = insFileName;
}

bool VPianoSettings::enableTouch() const
{
    return m_enableTouch;
}

void VPianoSettings::setEnableTouch(bool enableTouch)
{
    m_enableTouch = enableTouch;
}

bool VPianoSettings::enableMouse() const
{
    return m_enableMouse;
}

void VPianoSettings::setEnableMouse(bool enableMouse)
{
    m_enableMouse = enableMouse;
}

bool VPianoSettings::enableKeyboard() const
{
    return m_enableKeyboard;
}

void VPianoSettings::setEnableKeyboard(bool enableKeyboard)
{
    m_enableKeyboard = enableKeyboard;
}

bool VPianoSettings::enforceChannelState() const
{
    return m_enforceChannelState;
}

void VPianoSettings::setEnforceChannelState(bool enforceChannelState)
{
    m_enforceChannelState = enforceChannelState;
}

bool VPianoSettings::velocityColor() const
{
    return m_velocityColor;
}

void VPianoSettings::setVelocityColor(bool velocityColor)
{
    m_velocityColor = velocityColor;
}

bool VPianoSettings::rawKeyboard() const
{
    return m_rawKeyboard;
}

void VPianoSettings::setRawKeyboard(bool rawKeyboard)
{
    m_rawKeyboard = rawKeyboard;
}

bool VPianoSettings::alwaysOnTop() const
{
    return m_alwaysOnTop;
}

void VPianoSettings::setAlwaysOnTop(bool alwaysOnTop)
{
    m_alwaysOnTop = alwaysOnTop;
}

int VPianoSettings::drumsChannel() const
{
    return m_drumsChannel;
}

void VPianoSettings::setDrumsChannel(int drumsChannel)
{
    m_drumsChannel = drumsChannel;
}

QString VPianoSettings::language() const
{
    return m_language;
}

void VPianoSettings::setLanguage(const QString &language)
{
    m_language = language;
}

bool VPianoSettings::inputEnabled() const
{
    return m_inputEnabled;
}

void VPianoSettings::setInputEnabled(bool inputEnabled)
{
    m_inputEnabled = inputEnabled;
}

bool VPianoSettings::omniMode() const
{
    return m_omniMode;
}

void VPianoSettings::setOmniMode(bool omniMode)
{
    m_omniMode = omniMode;
}

QVariantMap VPianoSettings::settingsMap() const
{
    return m_settingsMap;
}

QString VPianoSettings::nativeOutput() const
{
    return m_defaultOutputBackend;
}

QString VPianoSettings::nativeInput() const
{
    return m_defaultInputBackend;
}

int VPianoSettings::startingKey() const
{
    return m_startingKey;
}

void VPianoSettings::setStartingKey(int startingKey)
{
    m_startingKey = startingKey;
}

int VPianoSettings::numKeys() const
{
    return m_numKeys;
}

void VPianoSettings::setNumKeys(int numKeys)
{
    m_numKeys = numKeys;
}

int VPianoSettings::baseOctave() const
{
    return m_baseOctave;
}

void VPianoSettings::setBaseOctave(int baseOctave)
{
    m_baseOctave = baseOctave;
}

int VPianoSettings::velocity() const
{
    return m_velocity;
}

void VPianoSettings::setVelocity(int velocity)
{
    m_velocity = velocity;
}

int VPianoSettings::channel() const
{
    return m_baseChannel;
}

void VPianoSettings::setChannel(int inChannel)
{
    m_baseChannel = inChannel;
}

bool VPianoSettings::advanced() const
{
    return m_advanced;
}

void VPianoSettings::setAdvanced(bool advanced)
{
    m_advanced = advanced;
}

bool VPianoSettings::midiThru() const
{
    return m_midiThru;
}

void VPianoSettings::setMidiThru(bool midiThru)
{
    m_midiThru = midiThru;
}

QString VPianoSettings::lastOutputConnection() const
{
    return m_lastOutputConnection;
}

void VPianoSettings::setLastOutputConnection(const QString &lastOutputConnection)
{
    m_lastOutputConnection = lastOutputConnection;
}

QString VPianoSettings::lastInputConnection() const
{
    return m_lastInputConnection;
}

void VPianoSettings::setLastInputConnection(const QString &lastInputConnection)
{
    m_lastInputConnection = lastInputConnection;
}

QString VPianoSettings::lastOutputBackend() const
{
    return m_lastOutputBackend;
}

void VPianoSettings::setLastOutputBackend(const QString &lastOutputBackend)
{
    m_lastOutputBackend = lastOutputBackend;
}

QString VPianoSettings::lastInputBackend() const
{
    return m_lastInputBackend;
}

void VPianoSettings::setLastInputBackend(const QString &lastInputBackend)
{
    m_lastInputBackend = lastInputBackend;
}

QByteArray VPianoSettings::state() const
{
    return m_state;
}

void VPianoSettings::setState(const QByteArray &state)
{
    m_state = state;
}

QByteArray VPianoSettings::geometry() const
{
    return m_geometry;
}

void VPianoSettings::setGeometry(const QByteArray &geometry)
{
    m_geometry = geometry;
}

Instrument* VPianoSettings::getInstrument()
{
    QString key = m_insName;
    if (key.isEmpty())
        return nullptr;
    if (!m_insList.contains(key))
        return nullptr;
    return &m_insList[key];
}

Instrument* VPianoSettings::getDrumsInstrument()
{
    QString key = m_insName;
    if (key.isEmpty())
        return nullptr;
    key.append(" Drums");
    if (!m_insList.contains(key))
        return nullptr;
    return &m_insList[key];
}

void VPianoSettings::setInstrumentsFileName( const QString fileName )
{
    QFileInfo f(fileName);
    if (f.isReadable()) {
        m_insList.clear();
        if (m_insList.load(fileName)) {
            setInsFileName( fileName );
            setInsName(m_insList.first().instrumentName());
        } else {
            setInsFileName( QString() );
            setInsName( QString() );
        }
    } else {
        qDebug() << "file" << fileName << "not readable.";
    }
}

QStringList VPianoSettings::getInsNames()
{
    QStringList result;
    if (!m_insList.isEmpty()) {
        InstrumentList::ConstIterator it;
        for(it = m_insList.constBegin(); it != m_insList.constEnd(); ++it) {
            if(!it.key().endsWith(QLatin1String("Drums"), Qt::CaseInsensitive))
                result.append(it.key());
        }
    }
    return  result;
}

bool VPianoSettings::savedShortcuts() const
{
    return m_shortcuts.count() > 0;
}

QString VPianoSettings::getShortcut(const QString& sKey) const
{
    return m_shortcuts.value(sKey);
}

QColor VPianoSettings::getColor(int i)
{
    return currentPalette()->getColor(i);
}

void VPianoSettings::saveCurrentPalette()
{
    currentPalette()->saveColors();
}

PianoPalette* VPianoSettings::currentPalette()
{
    if (m_currentPalette == 0) {
        m_currentPalette = &m_paletteList[PAL_SINGLE];
    }
    return m_currentPalette;
}

PianoPalette* VPianoSettings::getPalette(int pal)
{
    if (pal >= 0 && pal < m_paletteList.count()) {
        return &m_paletteList[pal];
    }
    return 0;
}

QList<QString> VPianoSettings::availablePaletteNames()
{
    QList<QString> tmp;
    foreach(const PianoPalette& p, m_paletteList) {
        tmp << p.paletteName();
    }
    return tmp;
}

void VPianoSettings::initializePalettes()
{
    resetPaletteSingle(&m_paletteList[PAL_SINGLE]);
    resetPaletteDouble(&m_paletteList[PAL_DOUBLE]);
    resetPaletteChannels(&m_paletteList[PAL_CHANNELS]);
    resetPaletteScale(&m_paletteList[PAL_SCALE]);
}

void VPianoSettings::resetPaletteSingle(PianoPalette *palette)
{
    palette->setColor(0, QString(), qApp->palette().highlight().color());
}

void VPianoSettings::resetPaletteDouble(PianoPalette *palette)
{
    palette->setColor(0, tr("N"), qApp->palette().highlight().color());
    palette->setColor(1, tr("#"), QColor("lawngreen"));
}

void VPianoSettings::resetPaletteChannels(PianoPalette *palette)
{
    palette->setColor(0, tr("1"), QColor("red"));
    palette->setColor(1, tr("2"), QColor("lime"));
    palette->setColor(2, tr("3"), QColor("blue"));
    palette->setColor(3, tr("4"), QColor("gold"));
    palette->setColor(4, tr("5"), QColor("maroon"));
    palette->setColor(5, tr("6"), QColor("green"));
    palette->setColor(6, tr("7"), QColor("navy"));
    palette->setColor(7, tr("8"), QColor("darkorange"));
    palette->setColor(8, tr("9"), QColor("purple"));
    palette->setColor(9, tr("10"), QColor());
    palette->setColor(10, tr("11"), QColor("teal"));
    palette->setColor(11, tr("12"), QColor("chocolate"));
    palette->setColor(12, tr("13"), QColor("fuchsia"));
    palette->setColor(13, tr("14"), QColor("olivedrab"));
    palette->setColor(14, tr("15"), QColor("aqua"));
    palette->setColor(15, tr("16"), QColor("greenyellow"));
}

void VPianoSettings::resetPaletteScale(PianoPalette *palette)
{
    /*
                    R       G       B
            C       -       -       100%    0
            C#      50%     -       100%    1
            D       100%    -       100%    2
            D#      100%    -       50%     3
            E       100%    -       -       4
            F       100%    50%     -       5
            F#      100%    100%    -       6
            G       50%     100%    -       7
            G#      -       100%    -       8
            A       -       100%    50%     9
            A#      -       100%    100%    10
            B       -       50%     100%    11
    */
    palette->setColor(0, tr("C"), QColor::fromRgb(0,0,255));
    palette->setColor(1, tr("C#"), QColor::fromRgb(127,0,255));
    palette->setColor(2, tr("D"), QColor::fromRgb(255,0,255));
    palette->setColor(3, tr("D#"), QColor::fromRgb(255,0,127));
    palette->setColor(4, tr("E"), QColor::fromRgb(255,0,0));
    palette->setColor(5, tr("F"), QColor::fromRgb(255,127,0));
    palette->setColor(6, tr("F#"), QColor::fromRgb(255,255,0));
    palette->setColor(7, tr("G"), QColor::fromRgb(127,255,0));
    palette->setColor(8, tr("G#"), QColor::fromRgb(0,255,0));
    palette->setColor(9, tr("A"), QColor::fromRgb(0,255,127));
    palette->setColor(10, tr("A#"), QColor::fromRgb(0,255,255));
    palette->setColor(11, tr("B"), QColor::fromRgb(0,127,255));
}

int VPianoSettings::availablePalettes() const
{
    return m_paletteList.length();
}

void VPianoSettings::setCurrentPalette(int i)
{
    if (i >= PAL_SINGLE && i < m_paletteList.length()) {
        m_currentPalette = getPalette(i);
        m_paletteId = i;
    }
}

void VPianoSettings::resetCurrentPalette()
{
    PianoPalette *palette = currentPalette();
    switch(palette->paletteId()) {
    case PAL_SINGLE:
        resetPaletteSingle(palette);
        break;
    case PAL_DOUBLE:
        resetPaletteDouble(palette);
        break;
    case PAL_CHANNELS:
        resetPaletteChannels(palette);
        break;
    case PAL_SCALE:
        resetPaletteScale(palette);
        break;
    default:
        return;
    }
}

void
VPianoSettings::retranslatePaletteSingle(PianoPalette *palette)
{
    palette->setColorName(0, QString());
}

void
VPianoSettings::retranslatePaletteDouble(PianoPalette *palette)
{
    palette->setColorName(0, tr("N"));
    palette->setColorName(1, tr("#"));
}

void
VPianoSettings::retranslatePaletteChannels(PianoPalette *palette)
{
    palette->setColorName(0, tr("1"));
    palette->setColorName(1, tr("2"));
    palette->setColorName(2, tr("3"));
    palette->setColorName(3, tr("4"));
    palette->setColorName(4, tr("5"));
    palette->setColorName(5, tr("6"));
    palette->setColorName(6, tr("7"));
    palette->setColorName(7, tr("8"));
    palette->setColorName(8, tr("9"));
    palette->setColorName(9, tr("10"));
    palette->setColorName(10, tr("11"));
    palette->setColorName(11, tr("12"));
    palette->setColorName(12, tr("13"));
    palette->setColorName(13, tr("14"));
    palette->setColorName(14, tr("15"));
    palette->setColorName(15, tr("16"));
}

void
VPianoSettings::retranslatePaletteScale(PianoPalette *palette)
{
    palette->setColorName(0, tr("C"));
    palette->setColorName(1, tr("C#"));
    palette->setColorName(2, tr("D"));
    palette->setColorName(3, tr("D#"));
    palette->setColorName(4, tr("E"));
    palette->setColorName(5, tr("F"));
    palette->setColorName(6, tr("F#"));
    palette->setColorName(7, tr("G"));
    palette->setColorName(8, tr("G#"));
    palette->setColorName(9, tr("A"));
    palette->setColorName(10, tr("A#"));
    palette->setColorName(11, tr("B"));
}

void
VPianoSettings::initializePaletteStrings()
{
    PianoPalette *palette;
    palette = &m_paletteList[PAL_SINGLE];
    palette->setPaletteName(tr("Single color"));
    palette->setPaletteText(tr("A single color to highlight all note events"));
    retranslatePaletteSingle(palette);

    palette = &m_paletteList[PAL_DOUBLE];
    palette->setPaletteName(tr("Two colors"));
    palette->setPaletteText(tr("One color to highlight natural notes and a different one for accidentals"));
    retranslatePaletteDouble(palette);

    palette = &m_paletteList[PAL_CHANNELS];
    palette->setPaletteName(tr("MIDI Channels"));
    palette->setPaletteText(tr("A different color for each MIDI channel. Enable Omni mode in the MIDI IN connection"));
    retranslatePaletteChannels(palette);

    palette = &m_paletteList[PAL_SCALE];
    palette->setPaletteName(tr("Chromatic scale"));
    palette->setPaletteText(tr("One color for each note in the chromatic scale"));
    retranslatePaletteScale(palette);
}

VMPKKeyboardMap *VPianoSettings::getKeyboardMap()
{
    return &m_keymap;
}

VMPKKeyboardMap *VPianoSettings::getRawKeyboardMap()
{
    return &m_rawmap;
}

