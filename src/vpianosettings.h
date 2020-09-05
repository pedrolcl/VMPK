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

#ifndef PORTABLESETTINGS_H
#define PORTABLESETTINGS_H

#include <QObject>
#include <QString>
#include <QSettings>
#include <drumstick/backendmanager.h>
#include <drumstick/pianopalette.h>
#include "instrument.h"
#include "keyboardmap.h"

using namespace drumstick::widgets;

class VPianoSettings : public QObject
{
    Q_OBJECT

public:
    static VPianoSettings* instance();

    QByteArray geometry() const;
    void setGeometry(const QByteArray &geometry);

    QByteArray state() const;
    void setState(const QByteArray &state);

    QString lastInputBackend() const;
    void setLastInputBackend(const QString &lastInputBackend);

    QString lastOutputBackend() const;
    void setLastOutputBackend(const QString &lastOutputBackend);

    QString lastInputConnection() const;
    void setLastInputConnection(const QString &lastInputConnection);

    QString lastOutputConnection() const;
    void setLastOutputConnection(const QString &lastOutputConnection);

    bool midiThru() const;
    void setMidiThru(bool midiThru);

    bool advanced() const;
    void setAdvanced(bool advanced);

    bool omniMode() const;
    void setOmniMode(bool omniMode);

    bool inputEnabled() const;
    void setInputEnabled(bool inputEnabled);

    int channel() const;
    void setChannel(int channel);

    int velocity() const;
    void setVelocity(int velocity);

    int baseOctave() const;
    void setBaseOctave(int baseOctave);

    int numKeys() const;
    void setNumKeys(int numKeys);

    int startingKey() const;
    void setStartingKey(int startingKey);

    QString nativeInput() const;
    QString nativeOutput() const;

    QVariantMap settingsMap() const;

    QString language() const;
    void setLanguage(const QString &language);

    int drumsChannel() const;
    void setDrumsChannel(int drumsChannel);

    bool alwaysOnTop() const;
    void setAlwaysOnTop(bool alwaysOnTop);

    bool rawKeyboard() const;
    void setRawKeyboard(bool rawKeyboard);

    bool velocityColor() const;
    void setVelocityColor(bool velocityColor);

    bool enforceChannelState() const;
    void setEnforceChannelState(bool enforceChannelState);

    bool enableKeyboard() const;
    void setEnableKeyboard(bool enableKeyboard);

    bool enableMouse() const;
    void setEnableMouse(bool enableMouse);

    bool enableTouch() const;
    void setEnableTouch(bool enableTouch);

    QString insFileName() const;
    void setInsFileName(const QString &insFileName);
    void setInstrumentsFileName( const QString fileName );
    Instrument* getInstrument();
    Instrument* getDrumsInstrument();
    QStringList getInsNames();

    int paletteId() const;
    void setPaletteId(int paletteId);

    int transpose() const;
    void setTranspose(int transpose);

    QString insName() const;
    void setInsName(const QString &insName);

    QString defaultInputConnection() const;
    QString defaultOutputConnection() const;

    bool showStatusBar() const;
    void setShowStatusBar(bool showStatusBar);

    bool colorScale() const;
    void setColorScale(bool colorScale);

    QString getMapFile() const;
    void setMapFile(const QString &mapFile);

    QString getRawMapFile() const;
    void setRawMapFile(const QString &rawMapFile);

    bool savedShortcuts() const;
    QString getShortcut(const QString& sKey) const;

    int availablePalettes() const;
    PianoPalette *currentPalette();
    PianoPalette *getPalette(int pal);
    QList<QString> availablePaletteNames();
    QColor getColor(int i);
    void saveCurrentPalette();
    void setCurrentPalette(int i);
    void resetCurrentPalette();

    VMPKKeyboardMap* getKeyboardMap();
    VMPKKeyboardMap* getRawKeyboardMap();

    void initializePalettes();
    void initializePaletteStrings();

    drumstick::widgets::PianoKeybd::LabelOrientation namesOrientation() const;
    void setNamesOrientation(drumstick::widgets::PianoKeybd::LabelOrientation namesOrientation);

    drumstick::widgets::PianoKeybd::LabelVisibility namesVisibility() const;
    void setNamesVisibility(drumstick::widgets::PianoKeybd::LabelVisibility namesVisibility);

    drumstick::widgets::PianoKeybd::LabelAlteration alterations() const;
    void setNamesAlterations(drumstick::widgets::PianoKeybd::LabelAlteration alterations);

    drumstick::widgets::PianoKeybd::LabelCentralOctave namesOctave() const;
    void setNamesOctave(drumstick::widgets::PianoKeybd::LabelCentralOctave namesOctave);

    QFont namesFont() const;
    void setNamesFont(const QFont &namesFont);

signals:
    void ValuesChanged();

public slots:
    void ResetDefaults();
    void ReadSettings();
    void ReadFromFile(const QString &filepath);
    void SaveSettings();
    void SaveToFile(const QString &filepath);

private:
    explicit VPianoSettings(QObject *parent = nullptr);

    void internalRead(QSettings &settings);
    void internalSave(QSettings &settings);
    void retranslatePaletteSingle(PianoPalette *palette);
    void retranslatePaletteDouble(PianoPalette *palette);
    void retranslatePaletteChannels(PianoPalette *palette);
    void retranslatePaletteScale(PianoPalette *palette);
    void resetPaletteSingle(PianoPalette *palette);
    void resetPaletteDouble(PianoPalette *palette);
    void resetPaletteChannels(PianoPalette *palette);
    void resetPaletteScale(PianoPalette *palette);

    QByteArray m_geometry;
    QByteArray m_state;
    QString m_lastInputBackend;
    QString m_lastOutputBackend;
    QString m_lastInputConnection;
    QString m_lastOutputConnection;
    bool m_midiThru;
    bool m_advanced;
    bool m_omniMode;
    bool m_inputEnabled;
    int m_baseChannel;
    int m_velocity;
    int m_baseOctave;
    int m_transpose;
    int m_numKeys;
    int m_startingKey;
    int m_paletteId;
    int m_drumsChannel;
    bool m_alwaysOnTop;
    bool m_rawKeyboard;
    bool m_velocityColor;
    bool m_enforceChannelState;
    bool m_enableKeyboard;
    bool m_enableMouse;
    bool m_enableTouch;
    bool m_showStatusBar;
    bool m_colorScale;
    QString m_insFileName;
    QString m_insName;
    drumstick::widgets::PianoKeybd::LabelVisibility m_namesVisibility;
    drumstick::widgets::PianoKeybd::LabelAlteration m_namesAlteration;
    drumstick::widgets::PianoKeybd::LabelCentralOctave m_namesOctave;
    drumstick::widgets::PianoKeybd::LabelOrientation m_namesOrientation;
    QFont m_namesFont;

    QString m_defaultInputBackend;
    QString m_defaultOutputBackend;
    QString m_defaultInputConnection;
    QString m_defaultOutputConnection;
    QString m_language;
    QVariantMap m_settingsMap;
    QVariantMap m_defaultsMap;
    InstrumentList m_insList;

    QString m_mapFile;
    QString m_rawMapFile;
    QMap<QString, QString> m_shortcuts;

    QList<PianoPalette> m_paletteList {
        PianoPalette(1, PAL_SINGLE),
        PianoPalette(2, PAL_DOUBLE),
        PianoPalette(16, PAL_CHANNELS),
        PianoPalette(12, PAL_SCALE),
    };
    PianoPalette *m_currentPalette;

    VMPKKeyboardMap m_keymap;
    VMPKKeyboardMap m_rawmap;
};

#endif // PORTABLESETTINGS_H
