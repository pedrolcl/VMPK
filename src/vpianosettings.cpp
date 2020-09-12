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

VPianoSettings::VPianoSettings(QObject *parent) : QObject(parent),
    m_currentPalette(PianoPalette(PAL_SINGLE))
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
    initializePaletteStrings();
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
    m_numKeys = DEFAULTNUMBEROFKEYS;
    m_startingKey = 9;
    m_defaultsMap = QVariantMap{
        { drumstick::rt::QSTR_DRUMSTICKRT_PUBLICNAMEIN, QSTR_VMPKINPUT},
        { drumstick::rt::QSTR_DRUMSTICKRT_PUBLICNAMEOUT, QSTR_VMPKOUTPUT}
    };

    m_paletteId = drumstick::widgets::PAL_SINGLE;
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

    loadPalettes();
    setCurrentPalette(m_paletteId);

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

    settings.beginGroup("TextSettings");
    QFont f;
    if (f.fromString(settings.value("namesFont", "Helvetica, 50").toString())) {
        setNamesFont(f);
    }
    setNamesOrientation(static_cast<PianoKeybd::LabelOrientation>(settings.value("namesOrientation", PianoKeybd::HorizontalOrientation).toInt()));
    setNamesVisibility(static_cast<PianoKeybd::LabelVisibility>(settings.value("namesVisibility", PianoKeybd::ShowNever).toInt()));
    setNamesAlterations(static_cast<PianoKeybd::LabelAlteration>(settings.value("namesAlteration", PianoKeybd::ShowSharps).toInt()));
    setNamesOctave(static_cast<PianoKeybd::LabelCentralOctave>(settings.value("namesOctave", PianoKeybd::OctaveC4).toInt()));
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

    settings.beginGroup("TextSettings");
    settings.setValue("namesFont", m_namesFont.toString());
    settings.setValue("namesOrientation", m_namesOrientation);
    settings.setValue("namesVisibility", m_namesVisibility);
    settings.setValue("namesAlteration", m_namesAlteration);
    settings.setValue("namesOctave", m_namesOctave);
    settings.endGroup();

    savePalettes();
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
    return currentPalette().getColor(i);
}

PianoPalette& VPianoSettings::currentPalette()
{
    return m_currentPalette;
}

PianoPalette& VPianoSettings::getPalette(int pal)
{
    if (pal >= 0 && pal < m_paletteList.count()) {
        return m_paletteList[pal];
    }
    return m_paletteList[0];
}

QList<QString> VPianoSettings::availablePaletteNames(bool forHighlight)
{
    QList<QString> tmp;
    for (PianoPalette& p : m_paletteList) {
        if (forHighlight && p.paletteId() >= PAL_SCALE) {
            continue;
        }
        tmp << p.paletteName();
    }
    return tmp;
}

void VPianoSettings::initializePalettes()
{
    for (PianoPalette& pal : m_paletteList) {
        pal.resetColors();
    }
}

int VPianoSettings::availablePalettes() const
{
    return m_paletteList.length();
}

void VPianoSettings::setCurrentPalette(int i)
{
    if (i >= PAL_SINGLE && i < PAL_SCALE) {
        m_currentPalette = getPalette(i);
        m_paletteId = i;
    }
}

void VPianoSettings::updatePalette(const PianoPalette& p)
{
    int id = p.paletteId();
    m_paletteList[id] = p;
}

void VPianoSettings::resetPalette(int pal)
{
    getPalette(pal).resetColors();
}

void
VPianoSettings::initializePaletteStrings()
{
    for (PianoPalette& pal : m_paletteList) {
        pal.retranslateStrings();
    }
}

void VPianoSettings::loadPalettes()
{
    for (PianoPalette& pal : m_paletteList) {
        pal.loadColors();
    }
}

void VPianoSettings::savePalettes()
{
    for (PianoPalette& pal : m_paletteList) {
        pal.saveColors();
    }
}

VMPKKeyboardMap *VPianoSettings::getKeyboardMap()
{
    return &m_keymap;
}

VMPKKeyboardMap *VPianoSettings::getRawKeyboardMap()
{
    return &m_rawmap;
}

QFont VPianoSettings::namesFont() const
{
    return m_namesFont;
}

void VPianoSettings::setNamesFont(const QFont &namesFont)
{
    m_namesFont = namesFont;
}

drumstick::widgets::PianoKeybd::LabelAlteration VPianoSettings::alterations() const
{
    return m_namesAlteration;
}

void VPianoSettings::setNamesAlterations(drumstick::widgets::PianoKeybd::LabelAlteration alterations)
{
    m_namesAlteration = alterations;
}

PianoKeybd::LabelVisibility VPianoSettings::namesVisibility() const
{
    return m_namesVisibility;
}

void VPianoSettings::setNamesVisibility(PianoKeybd::LabelVisibility namesVisibility)
{
    m_namesVisibility = namesVisibility;
}

PianoKeybd::LabelOrientation VPianoSettings::namesOrientation() const
{
    return m_namesOrientation;
}

void VPianoSettings::setNamesOrientation(PianoKeybd::LabelOrientation namesOrientation)
{
    m_namesOrientation = namesOrientation;
}

PianoKeybd::LabelCentralOctave VPianoSettings::namesOctave() const
{
    return m_namesOctave;
}

void VPianoSettings::setNamesOctave(PianoKeybd::LabelCentralOctave namesOctave)
{
    m_namesOctave = namesOctave;
}
