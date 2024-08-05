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

#ifndef MIDISETUP_H
#define MIDISETUP_H

#include <QDialog>
#include <drumstick/rtmidiinput.h>
#include <drumstick/rtmidioutput.h>
#include "ui_midisetup.h"

using namespace drumstick::rt;

class MidiSetup : public QDialog
{
    Q_OBJECT

public:
    MidiSetup(QWidget *parent = nullptr);
    void inputNotAvailable();
    void clearCombos();
    void retranslateUi();

    void setInput(MIDIInput *in);
    void setOutput(MIDIOutput *out);
    void setInputs(QList<MIDIInput *> ins);
    void setOutputs(QList<MIDIOutput *> outs);
    MIDIInput *getInput() { return m_midiIn; }
    MIDIOutput *getOutput() { return m_midiOut; }
    void showEvent(QShowEvent *) override;

public slots:
    void clickedAdvanced(bool value);
    void setMidiThru(bool value);
    void toggledInput(bool state);
    void refreshInputs(int idx);
    void refreshOutputs(int idx);
    void configureInput();
    void configureOutput();
    void refresh();
    void reopen();
    void accept() override;
    void reject() override;

private:
    void refreshInputDrivers(QString id, bool advanced);
    void refreshOutputDrivers(QString id, bool advanced);

    bool m_settingsChanged;
    Ui::MidiSetupClass ui;
    MIDIInput* m_midiIn, *m_savedIn;
    MIDIOutput* m_midiOut, *m_savedOut;
    MIDIConnection m_connIn, m_connOut;
};

#endif /* MIDISETUP_H */
