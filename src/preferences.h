/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2024, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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
#include <QDialog>

class ColorDialog;

class Preferences : public QDialog
{
    Q_OBJECT

public:
    Preferences(QWidget *parent = nullptr);
    void setInstrumentsFileName( const QString fileName );
    void setInstrumentName( const QString name );
    void apply();
    void setRawKeyMapFileName( const QString fileName );
    void setKeyMapFileName( const QString fileName );
    void retranslateUi();
    void setNoteNames(const QStringList& noteNames);
    void populateStyles();
    QString fontString(const QFont& f) const;

public slots:
    void slotOpenInstrumentFile();
    void slotSelectColor();
    void slotOpenKeymapFile();
    void slotOpenRawKeymapFile();
    void slotSelectFont();
    void slotRestoreDefaults();
    void accept() override;

protected:
    void showEvent ( QShowEvent *event ) override;

private:
    QString m_mapFile;
    QString m_rawMapFile;
    QString m_insFile;
    QFont   m_font;
    Ui::PreferencesClass ui;
};

#endif // PREFERENCES_H
