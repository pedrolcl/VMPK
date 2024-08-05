/*
 MIDI Virtual Piano Keyboard
 Copyright (C) 2008-2024, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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
#include <QTranslator>

#include <drumstick/backendmanager.h>
#include <drumstick/pianopalette.h>
#include "instrument.h"
#include "keyboardmap.h"

using namespace drumstick::widgets;

class VPianoSettings : public QObject
{
    Q_OBJECT

public:
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

    bool getWinSnap() const;
    void setWinSnap(bool winSnap);

    bool getDarkMode() const;
    void setDarkMode(bool darkMode);

    QString insFileName() const;
    QString insName() const;
    void setInstruments( const QString fileName, const QString name );
    Instrument* getInstrument();
    Instrument* getDrumsInstrument();
    QStringList getInsNames();

    int highlightPaletteId() const;
    void setHighlightPaletteId(int paletteId);

    int transpose() const;
    void setTranspose(int transpose);

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

    bool octaveSubscript() const;
    void setOctaveSubscript(bool newOctaveSubscript);

    int availablePalettes() const;
    PianoPalette getPalette(int pal);
    QList<QString> availablePaletteNames(bool forHighlight);
    void retranslatePalettes();
    void updatePalette(const PianoPalette &p);

    VMPKKeyboardMap* getKeyboardMap();
    VMPKKeyboardMap* getRawKeyboardMap();

    LabelOrientation namesOrientation() const;
    void setNamesOrientation(const LabelOrientation namesOrientation);

    LabelVisibility namesVisibility() const;
    void setNamesVisibility(const LabelVisibility namesVisibility);

    LabelAlteration alterations() const;
    void setNamesAlterations(const LabelAlteration alterations);

    LabelCentralOctave namesOctave() const;
    void setNamesOctave(const LabelCentralOctave namesOctave);

    QFont namesFont() const;
    void setNamesFont(const QFont &namesFont);

    QString getStyle() const;
    void setStyle(const QString &style);

    void loadTranslations();

    // static methods
    static VPianoSettings* instance();
    static void setPortableConfig(const QString fileName = QString());
    static QString dataDirectory();
    static QString localeDirectory();
    static QString drumstickLocales();
    static QString systemLocales();

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
    void initializePalettes();
    void loadPalettes();
    void savePalettes();
    QString fontString(const QFont &f) const;

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
    int m_highlightPaletteId;
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
    bool m_winSnap;
    bool m_darkMode;
    bool m_octaveSubscript;
    QString m_insFileName;
    QString m_insName;
    LabelVisibility m_namesVisibility;
    LabelAlteration m_namesAlteration;
    LabelCentralOctave m_namesOctave;
    LabelOrientation m_namesOrientation;
    QFont m_namesFont;

    QString m_language;
    QVariantMap m_settingsMap;
    QVariantMap m_defaultsMap;
    InstrumentList m_insList;

    QString m_mapFile;
    QString m_rawMapFile;
    QMap<QString, QString> m_shortcuts;

    QList<PianoPalette> m_paletteList {
        PianoPalette(PAL_SINGLE),
        PianoPalette(PAL_DOUBLE),
        PianoPalette(PAL_CHANNELS),
        PianoPalette(PAL_SCALE),
        PianoPalette(PAL_KEYS),
        PianoPalette(PAL_FONT),
        PianoPalette(PAL_HISCALE)
    };

    VMPKKeyboardMap m_keymap;
    VMPKKeyboardMap m_rawmap;
    QString m_style;

    QPointer<QTranslator> m_trq, m_trp, m_trl;
};

#endif // PORTABLESETTINGS_H
