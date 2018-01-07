/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2018, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QSettings>
#include <QNetworkInterface>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QDir>
#include <QStandardPaths>
#include <QFileDialog>
#include "macsynthsettingsdialog.h"
#include "ui_macsynthsettingsdialog.h"

MacSynthSettingsDialog::MacSynthSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MacSynthSettingsDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::pressed,
            this, &MacSynthSettingsDialog::restoreDefaults);
    connect(ui->btn_soundfont, &QToolButton::pressed, this, &MacSynthSettingsDialog::showFileDialog);
}

MacSynthSettingsDialog::~MacSynthSettingsDialog()
{
    delete ui;
}

void MacSynthSettingsDialog::accept()
{
    writeSettings();
    QDialog::accept();
}

void MacSynthSettingsDialog::showEvent(QShowEvent *event)
{
    readSettings();
    event->accept();
}

void MacSynthSettingsDialog::readSettings()
{

    QSettings settings;
    settings.beginGroup("DLS Synth");
    bool reverb = settings.value("reverb_dls", false).toBool();
    bool def = settings.value("default_dls", true).toBool();
    QString soundfont = settings.value("soundfont_dls").toString();
    settings.endGroup();

    ui->reverb_dls->setChecked(reverb);
    ui->default_dls->setChecked(def);
    ui->soundfont_dls->setText(soundfont);
}

void MacSynthSettingsDialog::writeSettings()
{
    QSettings settings;

    QString soundfont = ui->soundfont_dls->text();
    bool reverb = ui->reverb_dls->isChecked();
    bool def = ui->default_dls->isChecked();

    settings.beginGroup("DLS Synth");
    settings.setValue("soundfont_dls", soundfont);
    settings.setValue("reverb_dls", reverb);
    settings.setValue("default_dls", def);
    settings.endGroup();
    settings.sync();
}

void MacSynthSettingsDialog::restoreDefaults()
{
    ui->reverb_dls->setChecked(false);
    ui->default_dls->setChecked(true);
    ui->soundfont_dls->clear();
}

void MacSynthSettingsDialog::changeSoundFont(const QString& fileName)
{
    readSettings();
    ui->soundfont_dls->setText(fileName);
    writeSettings();
}

void MacSynthSettingsDialog::showFileDialog()
{
    QDir dir = (QDir::homePath() + "/Library/Audio/Sounds/Banks/");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select SoundFont"), dir.absolutePath(), tr("SoundFont Files (*.sf2 *.dls)"));
    if (!fileName.isEmpty()) {
        ui->soundfont_dls->setText(fileName);
    }
}
