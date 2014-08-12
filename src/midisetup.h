/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2014, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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
    MidiSetup(QWidget *parent = 0);
    void inputNotAvailable();
    bool inputIsEnabled() const;
    bool thruIsEnabled() const;
    bool omniIsEnabled() const;
    void setInputEnabled(const bool state);
    void setThruEnabled(const bool state);
    void setOmniEnabled(const bool state);
    void clearCombos();
    void retranslateUi();

    void setInput(MIDIInput *in) { m_midiIn = in; }
    void setOutput(MIDIOutput *out) { m_midiOut = out; }
    void setInputs(QList<MIDIInput *> ins);
    void setOutputs(QList<MIDIOutput *> outs);
    MIDIInput *getInput() { return m_midiIn; }
    MIDIOutput *getOutput() { return m_midiOut; }
    void setAdvanced(bool value);
    bool advanced();
    bool midiThru();

public slots:
    void clickedAdvanced(bool value);
    void setMidiThru(bool value);
    void toggledInput(bool state);
    void refreshInputs(QString id);
    void refreshOutputs(QString id);
    void configureInput();
    void configureOutput();
    void refresh();
    void accept();

private:
    bool m_advanced;
    bool m_thru;
    bool m_settingsChanged;
    Ui::MidiSetupClass ui;
    MIDIInput* m_midiIn;
    MIDIOutput* m_midiOut;
};

#endif /* MIDISETUP_H */
