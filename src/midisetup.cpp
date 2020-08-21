/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2020, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <QDebug>
#include <drumstick/configurationdialogs.h>
#include <drumstick/settingsfactory.h>
#include "midisetup.h"
#include "vpianosettings.h"

MidiSetup::MidiSetup(QWidget *parent) : QDialog(parent),
    m_settingsChanged(false),
    m_midiIn(0),
    m_midiOut(0)
{
    ui.setupUi(this);
    connect(ui.chkEnableInput, &QCheckBox::toggled, this, &MidiSetup::toggledInput);
    connect(ui.chkAdvanced, &QCheckBox::clicked, this, &MidiSetup::clickedAdvanced);
    connect(ui.comboinputBackends, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MidiSetup::refreshInputs);
    connect(ui.comboOutputBackends, QOverload<int>::of(&QComboBox::currentIndexChanged), this,  &MidiSetup::refreshOutputs);
    connect(ui.btnConfigInput, &QToolButton::clicked, this, &MidiSetup::configureInput);
    connect(ui.btnConfigOutput, &QToolButton::clicked, this, &MidiSetup::configureOutput);
}

void MidiSetup::toggledInput(bool state)
{
    ui.chkOmni->setEnabled(state);
    ui.chkEnableThru->setEnabled(state);
    ui.comboinputBackends->setEnabled(state);
    ui.comboInput->setEnabled(state);
    if (state) {
        refresh();
    } else {
        ui.chkOmni->setChecked(false);
        ui.chkEnableThru->setChecked(false);
        ui.comboinputBackends->setCurrentIndex(-1);
        ui.comboInput->setCurrentIndex(-1);
    }
}

void MidiSetup::inputNotAvailable()
{
    toggledInput(false);
}

void MidiSetup::clearCombos()
{
    ui.comboInput->clear();
    ui.comboOutput->clear();
}

void MidiSetup::retranslateUi()
{
    ui.retranslateUi(this);
}

void MidiSetup::setInputs(QList<MIDIInput *> ins)
{
    ui.comboinputBackends->disconnect();
    ui.comboinputBackends->clear();
    foreach(MIDIInput *i, ins) {
        ui.comboinputBackends->addItem(i->backendName(), QVariant::fromValue(i));
    }
    connect(ui.comboinputBackends, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MidiSetup::refreshInputs);
}

void MidiSetup::setOutputs(QList<MIDIOutput *> outs)
{
    ui.comboOutputBackends->disconnect();
    foreach(MIDIOutput *o, outs) {
        ui.comboOutputBackends->addItem(o->backendName(), QVariant::fromValue(o));
    }
    connect(ui.comboOutputBackends, QOverload<int>::of(&QComboBox::currentIndexChanged), this,  &MidiSetup::refreshOutputs);
}

void MidiSetup::showEvent(QShowEvent *)
{
    ui.chkEnableInput->setChecked(VPianoSettings::instance()->inputEnabled());
    ui.chkEnableThru->setChecked(VPianoSettings::instance()->midiThru());
    ui.chkAdvanced->setChecked(VPianoSettings::instance()->advanced());
    ui.chkOmni->setChecked(VPianoSettings::instance()->omniMode());
}

void MidiSetup::accept()
{
    QString conn;
    drumstick::widgets::SettingsFactory settings;
    VPianoSettings::instance()->setAdvanced(ui.chkAdvanced->isChecked());
    VPianoSettings::instance()->setMidiThru(ui.chkEnableThru->isChecked());
    VPianoSettings::instance()->setOmniMode(ui.chkOmni->isChecked());
    VPianoSettings::instance()->setInputEnabled(ui.chkEnableInput->isChecked());
    if (m_midiOut != 0) {
        conn = ui.comboOutput->currentText();
        if (conn != m_midiOut->currentConnection() || m_settingsChanged) {
            m_midiOut->close();
            if (!conn.isEmpty()) {
                m_midiOut->initialize(settings.getQSettings());
                m_midiOut->open(conn);
            }
        }
    }
    if (m_midiIn != 0) {
        conn = ui.comboInput->currentText();
        if (conn != m_midiIn->currentConnection() || m_settingsChanged) {
            m_midiIn->close();
            if (!conn.isEmpty()) {
                m_midiIn->initialize(settings.getQSettings());
                m_midiIn->open(conn);
            }
        }
        m_midiIn->enableMIDIThru(ui.chkEnableThru->isChecked());
        m_midiIn->setMIDIThruDevice(m_midiOut);
    }
    m_settingsChanged = false;
    QDialog::accept();
}

void MidiSetup::refresh()
{
    bool advanced = ui.chkAdvanced->isChecked();
    if (m_midiIn != 0) {
        ui.comboinputBackends->setCurrentText(m_midiIn->backendName());
        refreshInputDrivers(m_midiIn->backendName(), advanced);
    }
    if (m_midiOut != 0) {
        ui.comboOutputBackends->setCurrentText(m_midiOut->backendName());
        refreshOutputDrivers(m_midiOut->backendName(), advanced);
    }
}

void MidiSetup::refreshInputs(int idx)
{
    bool advanced = ui.chkAdvanced->isChecked();
    QString id = ui.comboinputBackends->itemText(idx);
    refreshInputDrivers(id, advanced);
}

void MidiSetup::refreshInputDrivers(QString id, bool advanced)
{
    //qDebug() << Q_FUNC_INFO << id;
    ui.btnConfigInput->setEnabled(id == "Network");
    if (m_midiIn != 0 && m_midiIn->backendName() != id) {
        m_midiIn->close();
        int idx = ui.comboinputBackends->findText(id, Qt::MatchStartsWith);
        if (idx > -1)
            m_midiIn = ui.comboinputBackends->itemData(idx).value<MIDIInput*>();
        else
            m_midiIn = 0;
    }
    ui.comboInput->clear();
    if (m_midiIn != 0) {
        ui.comboInput->addItem(QString());
        ui.comboInput->addItems(m_midiIn->connections(advanced));
        ui.comboInput->setCurrentText(m_midiIn->currentConnection());
    }
}

void MidiSetup::refreshOutputs(int idx)
{
    bool advanced = ui.chkAdvanced->isChecked();
    QString id = ui.comboOutputBackends->itemText(idx);
    refreshOutputDrivers(id, advanced);
}

void MidiSetup::refreshOutputDrivers(QString id, bool advanced)
{
    //qDebug() << Q_FUNC_INFO << id;
    ui.btnConfigOutput->setEnabled(id == "Network" || id == "FluidSynth" || id == "SonivoxEAS" || id == "DLS Synth");
    if (m_midiOut != 0 && m_midiOut->backendName() != id) {
        m_midiOut->close();
        int idx = ui.comboOutputBackends->findText(id, Qt::MatchStartsWith);
        if (idx > -1)
            m_midiOut = ui.comboOutputBackends->itemData(idx).value<MIDIOutput*>();
        else
            m_midiOut = 0;
    }
    ui.comboOutput->clear();
    if (m_midiOut != 0) {
        ui.comboOutput->addItems(m_midiOut->connections(advanced));
        ui.comboOutput->setCurrentText(m_midiOut->currentConnection());
    }
}

void MidiSetup::configureInput()
{
    QString driver = ui.comboinputBackends->currentText();
    if (driver == "Network") {
        m_settingsChanged = drumstick::widgets::configureInputDriver(driver, this);
    }
}

void MidiSetup::configureOutput()
{
    QString driver = ui.comboOutputBackends->currentText();
    if (driver == "Network" ||
        driver == "FluidSynth" ||
        driver == "DLS Synth" ||
        driver == "SonivoxEAS") {
        m_settingsChanged = drumstick::widgets::configureOutputDriver(driver, this);
    }
}

void MidiSetup::clickedAdvanced(bool value)
{
    Q_UNUSED(value)
    refresh();
}

void MidiSetup::setMidiThru(bool value)
{
    ui.chkEnableThru->setChecked(value);
}
