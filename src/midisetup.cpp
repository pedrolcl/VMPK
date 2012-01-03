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

#include "midisetup.h"

MidiSetup::MidiSetup(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    connect(ui.chkEnableInput, SIGNAL(toggled(bool)), SLOT(toggledInput(bool)));
#if defined(__LINUX_ALSASEQ__) || defined(__MACOSX_CORE__)
    ui.chkEnableInput->setEnabled(false);
#endif
}

void MidiSetup::toggledInput(bool state)
{
    if (!state) {
        ui.chkEnableThru->setChecked(false);
        ui.comboInput->setCurrentIndex(-1);
    }
}

void MidiSetup::inputNotAvailable()
{
    setInputEnabled(false);
    ui.chkEnableInput->setEnabled(false);
    ui.chkEnableThru->setEnabled(false);
    ui.comboInput->setCurrentIndex(-1);
}

bool MidiSetup::inputIsEnabled() const
{
    return ui.chkEnableInput->isChecked();
}

bool MidiSetup::thruIsEnabled() const
{
    return ui.chkEnableThru->isChecked();
}

void MidiSetup::setInputEnabled(const bool state)
{
    ui.chkEnableInput->setChecked(state);
}

void MidiSetup::setThruEnabled(const bool state)
{
    ui.chkEnableThru->setChecked(state);
}

void MidiSetup::clearCombos()
{
    ui.comboInput->clear();
    ui.comboOutput->clear();
}

void MidiSetup::addInputPortName(const QString& input, int index)
{
    ui.comboInput->addItem(input, index);
}

void MidiSetup::setCurrentInput(int index)
{
    if (index < 0)
        ui.comboInput->setCurrentIndex(index);
    else {
        int i;
        for (i = 0; i < ui.comboInput->count(); ++i) {
            if (index == ui.comboInput->itemData(i).toInt()) {
                ui.comboInput->setCurrentIndex(i);
                return;
            }
        }
    }
}

void MidiSetup::setCurrentInput(const QString name)
{
    int i;
    for (i = 0; i < ui.comboInput->count(); ++i) {
        if (name == ui.comboInput->itemText(i)) {
            ui.comboInput->setCurrentIndex(i);
            return;
        }
    }
    ui.comboInput->setCurrentIndex(-1);
}

void MidiSetup::setCurrentOutput(int index)
{
    if (index < 0)
        ui.comboOutput->setCurrentIndex(index);
    else {
        int i;
        for (i = 0; i < ui.comboOutput->count(); ++i) {
            if (index == ui.comboOutput->itemData(i).toInt()) {
                ui.comboOutput->setCurrentIndex(i);
                return;
            }
        }
    }
}

void MidiSetup::setCurrentOutput(const QString name)
{
    int i;
    for (i = 0; i < ui.comboOutput->count(); ++i) {
        if (name == ui.comboOutput->itemText(i)) {
            ui.comboOutput->setCurrentIndex(i);
            return;
        }
    }
    ui.comboOutput->setCurrentIndex(-1);
}

void MidiSetup::addOutputPortName(const QString& output, int index)
{
    ui.comboOutput->addItem(output, index);
}

int MidiSetup::selectedInput()
{
    int idx = ui.comboInput->currentIndex();
    if (idx >= 0)
        return ui.comboInput->itemData(idx).toInt();
    else
        return -1;
}

int MidiSetup::selectedOutput()
{
    int idx = ui.comboOutput->currentIndex();
    if (idx >= 0)
        return ui.comboOutput->itemData(idx).toInt();
    else
        return -1;
}

QString MidiSetup::selectedInputName() const
{
    int idx = ui.comboInput->currentIndex();
    if (idx >= 0)
        return ui.comboInput->itemText(idx);
    else
        return QString();
}

QString MidiSetup::selectedOutputName() const
{
    int idx = ui.comboOutput->currentIndex();
    if (idx >= 0)
        return ui.comboOutput->itemText(idx);
    else
        return QString();
}

void MidiSetup::retranslateUi()
{
    ui.retranslateUi(this);
}
