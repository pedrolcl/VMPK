/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2017, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <QSettings>
#include <QDebug>
#include "midisetup.h"
#include "fluidsettingsdialog.h"
#include "networksettingsdialog.h"
#include "macsynthsettingsdialog.h"
#include "sonivoxsettingsdialog.h"

MidiSetup::MidiSetup(QWidget *parent) : QDialog(parent),
    m_advanced(false),
    m_thru(false),
    m_settingsChanged(false),
    m_midiIn(0),
    m_midiOut(0)
{
    ui.setupUi(this);
    connect(ui.chkEnableInput, SIGNAL(toggled(bool)), SLOT(toggledInput(bool)));
    connect(ui.chkAdvanced, SIGNAL(clicked(bool)), SLOT(clickedAdvanced(bool)));
    connect(ui.comboinputBackends, SIGNAL(currentIndexChanged(QString)), SLOT(refreshInputs(QString)));
    connect(ui.comboOutputBackends, SIGNAL(currentIndexChanged(QString)), SLOT(refreshOutputs(QString)));
    connect(ui.btnConfigInput, &QToolButton::clicked, this, &MidiSetup::configureInput);
    connect(ui.btnConfigOutput, &QToolButton::clicked, this, &MidiSetup::configureOutput);
    ui.chkAdvanced->setChecked(m_advanced);
    ui.chkEnableThru->setChecked(m_thru);
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
    setInputEnabled(false);
    toggledInput(false);
}

bool MidiSetup::inputIsEnabled() const
{
    return ui.chkEnableInput->isChecked();
}

bool MidiSetup::thruIsEnabled() const
{
    return ui.chkEnableThru->isChecked();
}

bool MidiSetup::omniIsEnabled() const
{
    return ui.chkOmni->isChecked();
}

void MidiSetup::setInputEnabled(const bool state)
{
    ui.chkEnableInput->setChecked(state);
}

void MidiSetup::setThruEnabled(const bool state)
{
    ui.chkEnableThru->setChecked(state);
}

void MidiSetup::setOmniEnabled(const bool state)
{
    ui.chkOmni->setChecked(state);
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
        ui.comboinputBackends->addItem(i->backendName(), qVariantFromValue((void *) i));
    }
    connect(ui.comboinputBackends, SIGNAL(currentIndexChanged(QString)), SLOT(refreshInputs(QString)));
}

void MidiSetup::setOutputs(QList<MIDIOutput *> outs)
{
    ui.comboOutputBackends->disconnect();
    foreach(MIDIOutput *o, outs) {
        ui.comboOutputBackends->addItem(o->backendName(), qVariantFromValue((void *) o));
    }
    connect(ui.comboOutputBackends, SIGNAL(currentIndexChanged(QString)), SLOT(refreshOutputs(QString)));
}

void MidiSetup::accept()
{
    QString conn;
    QSettings settings;
    m_advanced = ui.chkAdvanced->isChecked();
    m_thru = ui.chkEnableThru->isChecked();
    if (m_midiOut != 0) {
        conn = ui.comboOutput->currentText();
        if (conn != m_midiOut->currentConnection() || m_settingsChanged) {
            m_midiOut->close();
            if (!conn.isEmpty()) {
                m_midiOut->initialize(&settings);
                m_midiOut->open(conn);
            }
        }
    }
    if (m_midiIn != 0) {
        conn = ui.comboInput->currentText();
        if (conn != m_midiIn->currentConnection() || m_settingsChanged) {
            m_midiIn->close();
            if (!conn.isEmpty()) {
                m_midiIn->initialize(&settings);
                m_midiIn->open(conn);
            }
        }
        m_midiIn->enableMIDIThru(m_thru);
        m_midiIn->setMIDIThruDevice(m_midiOut);
    }
    m_settingsChanged = false;
    QDialog::accept();
}

void MidiSetup::refresh()
{
    m_advanced = ui.chkAdvanced->isChecked();
    if (m_midiIn != 0) {
        ui.comboinputBackends->setCurrentText(m_midiIn->backendName());
        refreshInputs(m_midiIn->backendName());
    }
    if (m_midiOut != 0) {
        ui.comboOutputBackends->setCurrentText(m_midiOut->backendName());
        refreshOutputs(m_midiOut->backendName());
    }
}

void MidiSetup::refreshInputs(QString id)
{
    //qDebug() << Q_FUNC_INFO << id;
    ui.btnConfigInput->setEnabled(id == "Network");
    if (m_midiIn != 0 && m_midiIn->backendName() != id) {
        m_midiIn->close();
        int idx = ui.comboinputBackends->findText(id, Qt::MatchStartsWith);
        if (idx > -1)
            m_midiIn = (MIDIInput *) ui.comboinputBackends->itemData(idx).value<void *>();
        else
            m_midiIn = 0;
    }
    ui.comboInput->clear();
    if (m_midiIn != 0) {
        ui.comboInput->addItem(QString());
        ui.comboInput->addItems(m_midiIn->connections(m_advanced));
        ui.comboInput->setCurrentText(m_midiIn->currentConnection());
    }
}

void MidiSetup::refreshOutputs(QString id)
{
    //qDebug() << Q_FUNC_INFO << id;
    ui.btnConfigOutput->setEnabled(id == "Network" || id == "FluidSynth" || id == "SonivoxEAS" || id == "DLS Synth");
    if (m_midiOut != 0 && m_midiOut->backendName() != id) {
        m_midiOut->close();
        int idx = ui.comboOutputBackends->findText(id, Qt::MatchStartsWith);
        if (idx > -1)
            m_midiOut = (MIDIOutput *) ui.comboOutputBackends->itemData(idx).value<void *>();
        else
            m_midiOut = 0;
    }
    ui.comboOutput->clear();
    if (m_midiOut != 0) {
        ui.comboOutput->addItems(m_midiOut->connections(m_advanced));
        ui.comboOutput->setCurrentText(m_midiOut->currentConnection());
    }
}

void MidiSetup::configureInput()
{
    QString driver = ui.comboinputBackends->currentText();
    if (driver == "Network") {
        NetworkSettingsDialog dlg(this);
        m_settingsChanged = ( dlg.exec() == QDialog::Accepted );
    }
}

void MidiSetup::configureOutput()
{
    QString driver = ui.comboOutputBackends->currentText();
    if (driver == "Network") {
        NetworkSettingsDialog dlg(this);
        m_settingsChanged = ( dlg.exec() == QDialog::Accepted );
    } else if (driver == "FluidSynth") {
        FluidSettingsDialog dlg(this);
        m_settingsChanged = ( dlg.exec() == QDialog::Accepted );
    } else if (driver == "DLS Synth") {
        MacSynthSettingsDialog dlg(this);
        m_settingsChanged = ( dlg.exec() == QDialog::Accepted );
    } else if (driver == "SonivoxEAS") {
        SonivoxSettingsDialog dlg(this);
        m_settingsChanged = ( dlg.exec() == QDialog::Accepted );
    }
}

void MidiSetup::setAdvanced(bool value)
{
    ui.chkAdvanced->setChecked(value);
    refresh();
}

void MidiSetup::clickedAdvanced(bool value)
{
    m_advanced = value;
    refresh();
}

void MidiSetup::setMidiThru(bool value)
{
    m_thru = value;
    ui.chkEnableThru->setChecked(value);
}

bool MidiSetup::advanced()
{
    return ui.chkAdvanced->isChecked();
}

bool MidiSetup::midiThru()
{
    return ui.chkEnableThru->isChecked();
}

bool MidiSetup::changeSoundFont(const QString& fileName)
{
    QString driver = ui.comboOutputBackends->currentText();
    if (driver == "FluidSynth" || driver == "DLS Synth") {
        if (driver == "FluidSynth") {
            FluidSettingsDialog dlg(this);
            dlg.changeSoundFont(fileName);
        } else if (driver == "DLS Synth") {
            MacSynthSettingsDialog dlg(this);
            dlg.changeSoundFont(fileName);
        }
        if (m_midiOut != 0) {
            QSettings settings;
            QString conn = ui.comboOutput->currentText();
            if (!conn.isEmpty()) {
                m_midiOut->close();
                m_midiOut->initialize(&settings);
                m_midiOut->open(conn);
                return true;
            }
        }
    }
    return false;
}
