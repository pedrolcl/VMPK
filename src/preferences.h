/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2011, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "ui_preferences.h"
#include "instrument.h"
#include "keyboardmap.h"
#include <QtGui/QDialog>

class Preferences : public QDialog
{
    Q_OBJECT

public:
    Preferences(QWidget *parent = 0);
    int getNumOctaves() const { return m_numOctaves; }
    int getDrumsChannel() const { return m_drumsChannel; }
    int getNetworkPort() const { return m_networkPort; }
    QColor getKeyPressedColor() const { return m_keyPressedColor; }
    bool getGrabKeyboard() const { return m_grabKb; }
    bool getStyledWidgets() const { return m_styledKnobs; }
    bool getAlwaysOnTop() const { return m_alwaysOnTop; }
    bool getRawKeyboard() const { return m_rawKeyboard; }
    bool getVelocityColor() const { return m_velocityColor; }
    void setInstrumentsFileName( const QString fileName );
    QString getInstrumentsFileName();
    void setInstrumentName( const QString name );
    QString getInstrumentName();
    QString getNetworkInterface();
    void setNetworkIface(const QString iface);
    void apply();
    Instrument* getInstrument();
    Instrument* getDrumsInstrument();
    void setRawKeyMapFileName( const QString fileName );
    void setKeyMapFileName( const QString fileName );
    KeyboardMap* getKeyboardMap() { return &m_keymap; }
    KeyboardMap* getRawKeyboardMap() { return &m_rawmap; }
    void retranslateUi();

public slots:
    void setNumOctaves(int value) { m_numOctaves = value; }
    void setDrumsChannel(int value) { m_drumsChannel = value; }
    void setNetworkPort(int value) { m_networkPort = value; }
    void setGrabKeyboard(bool value) { m_grabKb = value; }
    void setStyledWidgets(bool value) { m_styledKnobs = value; }
    void setAlwaysOnTop(bool value) { m_alwaysOnTop = value; }
    void setRawKeyboard(bool value) { m_rawKeyboard = value; }
    void setVelocityColor(bool value) { m_velocityColor = value; }
    void setKeyPressedColor(QColor value);
    void slotOpenInstrumentFile();
    void slotSelectColor();
    void slotOpenKeymapFile();
    void slotOpenRawKeymapFile();
    void slotRestoreDefaults();
    void accept();

protected:
    void showEvent ( QShowEvent *event );
    void restoreDefaults();

private:
    Ui::PreferencesClass ui;
    QString m_insFileName;
    InstrumentList m_ins;
    int m_numOctaves;
    int m_drumsChannel;
    int m_networkPort;
    bool m_grabKb;
    bool m_styledKnobs;
    bool m_alwaysOnTop;
    bool m_rawKeyboard;
    bool m_velocityColor;
    QColor m_keyPressedColor;
    KeyboardMap m_keymap;
    KeyboardMap m_rawmap;
};

#endif // PREFERENCES_H
