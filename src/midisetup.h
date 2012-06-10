/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2012, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#ifndef MIDISETUP_H
#define MIDISETUP_H

#include "ui_midisetup.h"
#include <QtGui/QDialog>

class MidiSetup : public QDialog
{
    Q_OBJECT

public:
    MidiSetup(QWidget *parent = 0);
    void inputNotAvailable();
    bool inputIsEnabled() const;
    bool thruIsEnabled() const;
    bool omniIsEnabled() const;
    void setInputEnabled(const bool state);
    void setThruEnabled(const bool state);
    void setOmniEnabled(const bool state);
    void clearCombos();
    void addInputPortName(const QString& name, int index);
    void addOutputPortName(const QString& output, int index);
    void setCurrentInput(int index);
    void setCurrentInput(const QString name);
    void setCurrentOutput(int index);
    void setCurrentOutput(const QString name);
    int  selectedInput();
    int  selectedOutput();
    QString selectedInputName() const;
    QString selectedOutputName() const;
    void retranslateUi();

public slots:
    void toggledInput(bool state);

private:
    Ui::MidiSetupClass ui;
};

#endif /* MIDISETUP_H */
