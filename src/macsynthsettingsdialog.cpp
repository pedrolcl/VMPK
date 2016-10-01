/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2016, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <QSettings>
#include <QNetworkInterface>
#include <QDialogButtonBox>
#include <QPushButton>
#include "macsynthsettingsdialog.h"
#include "ui_macsynthsettingsdialog.h"

MacSynthSettingsDialog::MacSynthSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MacSynthSettingsDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::pressed,
            this, &MacSynthSettingsDialog::restoreDefaults);
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
    bool reverb = settings.value("reverb", false).toBool();
    bool def = settings.value("defaultdls", true).toBool();
    QString soundfont = settings.value("soundfont").toString();
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
    settings.setValue("soundfont", soundfont);
    settings.setValue("reverb", reverb);
    settings.setValue("defaultdls", def);
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
