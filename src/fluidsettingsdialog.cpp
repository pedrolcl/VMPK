/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2015, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QFileDialog>
#include <QToolButton>
#include <QPushButton>

#include "fluidsettingsdialog.h"
#include "ui_fluidsettingsdialog.h"

const QString QSTR_PREFERENCES("FluidSynth");
const QString QSTR_INSTRUMENTSDEFINITION("InstrumentsDefinition");
const QString QSTR_DATADIR("soundfonts");
const QString QSTR_SOUNDFONT("default.sf2");
const QString QSTR_AUDIODRIVER("AudioDriver");
const QString QSTR_PERIODSIZE("PeriodSize");
const QString QSTR_PERIODS("Periods");
const QString QSTR_SAMPLERATE("SampleRate");
const QString QSTR_CHORUS("Chorus");
const QString QSTR_REVERB("Reverb");
const QString QSTR_GAIN("Gain");
const QString QSTR_POLYPHONY("Polyphony");

const int DEFAULT_PERIODSIZE = 512;
const int DEFAULT_PERIODS = 3;
const double DEFAULT_SAMPLERATE = 48000.0;
const int DEFAULT_CHORUS = 0;
const int DEFAULT_REVERB = 0;
const double DEFAULT_GAIN = .4;
const int DEFAULT_POLYPHONY = 32;

FluidSettingsDialog::FluidSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FluidSettingsDialog)
{
    ui->setupUi(this);
    connect(ui->btnFile, &QToolButton::clicked, this, &FluidSettingsDialog::showFileDialog);
    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked,
            this, &FluidSettingsDialog::restoreDefaults);
    ui->periodSize->setValidator(new QIntValidator(64, 8192, this));
    ui->periods->setValidator(new QIntValidator(2, 64, this));
    ui->sampleRate->setValidator(new QDoubleValidator(22050.0, 96000.0, 1, this));
    ui->gain->setValidator(new QDoubleValidator(0.0, 10.0, 2, this));
    ui->polyphony->setValidator(new QIntValidator(16, 4096, this));
}

FluidSettingsDialog::~FluidSettingsDialog()
{
    delete ui;
}

void FluidSettingsDialog::accept()
{
    writeSettings();
    QDialog::accept();
}

void FluidSettingsDialog::showEvent(QShowEvent *event)
{
    readSettings();
    event->accept();
}

QString FluidSettingsDialog::defaultAudioDriver() const
{
    const QString QSTR_DEFAULT_AUDIODRIVER =
#if defined(Q_OS_LINUX)
        QLatin1Literal("alsa");
#elif defined(Q_OS_WIN)
        QLatin1Literal("dsound");
#elif defined(Q_OS_OSX)
        QLatin1Literal("coreaudio");
#else
        QLatin1Literal("oss");
#endif
    return QSTR_DEFAULT_AUDIODRIVER;
}

void FluidSettingsDialog::readSettings()
{
    QSettings settings;
    QStringList drivers;
    QString fs_defSoundFont = QSTR_SOUNDFONT;
#if defined(Q_OS_LINUX)
    drivers << "alsa" << "pulseaudio" << "oss";
#elif defined(Q_OS_WIN)
    drivers << "dsound";
#elif defined(Q_OS_OSX)
    drivers << "coreaudio";
#else
    drivers << "oss";
#endif

    QDir dir(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QSTR_DATADIR, QStandardPaths::LocateDirectory));
    QFileInfo sf2(dir, QSTR_SOUNDFONT);
    if (sf2.exists()) {
        fs_defSoundFont = sf2.absoluteFilePath();
    }

    ui->audioDriver->addItems(drivers);

    settings.beginGroup(QSTR_PREFERENCES);
    ui->audioDriver->setCurrentText( settings.value(QSTR_AUDIODRIVER, defaultAudioDriver()).toString() );
    ui->periodSize->setText( settings.value(QSTR_PERIODSIZE, DEFAULT_PERIODSIZE).toString() );
    ui->periods->setText( settings.value(QSTR_PERIODS, DEFAULT_PERIODS).toString() );
    ui->sampleRate->setText( settings.value(QSTR_SAMPLERATE, DEFAULT_SAMPLERATE).toString() );
    ui->chorus->setChecked( settings.value(QSTR_CHORUS, DEFAULT_CHORUS).toInt() != 0 );
    ui->reverb->setChecked( settings.value(QSTR_REVERB, DEFAULT_REVERB).toInt() != 0 );
    ui->gain->setText( settings.value(QSTR_GAIN, DEFAULT_GAIN).toString() );
    ui->polyphony->setText( settings.value(QSTR_POLYPHONY, DEFAULT_POLYPHONY).toString() );
    ui->soundFont->setText( settings.value(QSTR_INSTRUMENTSDEFINITION, fs_defSoundFont).toString() );
    settings.endGroup();
}

void FluidSettingsDialog::writeSettings()
{
    QSettings settings;
    QString audioDriver;
    QString soundFont(QSTR_SOUNDFONT);
    int     periodSize(DEFAULT_PERIODSIZE);
    int     periods(DEFAULT_PERIODS);
    double  sampleRate(DEFAULT_SAMPLERATE);
    int     chorus(DEFAULT_CHORUS);
    int     reverb(DEFAULT_REVERB);
    double  gain(DEFAULT_GAIN);
    int     polyphony(DEFAULT_POLYPHONY);

    audioDriver = ui->audioDriver->currentText();
    if (audioDriver.isEmpty()) {
        audioDriver = defaultAudioDriver();
    }
    soundFont = ui->soundFont->text();
    periodSize = ui->periodSize->text().toInt();
    periods = ui->periods->text().toInt();
    sampleRate = ui->sampleRate->text().toDouble();
    chorus = (ui->chorus->isChecked() ? 1 : 0);
    reverb = (ui->reverb->isChecked() ? 1 : 0);
    gain = ui->gain->text().toDouble();
    polyphony = ui->polyphony->text().toInt();

    settings.beginGroup(QSTR_PREFERENCES);
    settings.setValue(QSTR_INSTRUMENTSDEFINITION, soundFont);
    settings.setValue(QSTR_AUDIODRIVER, audioDriver);
    settings.setValue(QSTR_PERIODSIZE, periodSize);
    settings.setValue(QSTR_PERIODS, periods);
    settings.setValue(QSTR_SAMPLERATE, sampleRate);
    settings.setValue(QSTR_CHORUS, chorus);
    settings.setValue(QSTR_REVERB, reverb);
    settings.setValue(QSTR_GAIN, gain);
    settings.setValue(QSTR_POLYPHONY, polyphony);
    settings.endGroup();

    settings.sync();
}

void FluidSettingsDialog::restoreDefaults()
{
    ui->audioDriver->setCurrentText( defaultAudioDriver() );
    ui->periodSize->setText( QString::number( DEFAULT_PERIODSIZE ));
    ui->periods->setText( QString::number( DEFAULT_PERIODS ));
    ui->sampleRate->setText( QString::number( DEFAULT_SAMPLERATE ));
    ui->chorus->setChecked( DEFAULT_CHORUS != 0 );
    ui->reverb->setChecked( DEFAULT_REVERB != 0 );
    ui->gain->setText( QString::number( DEFAULT_GAIN ) );
    ui->polyphony->setText( QString::number( DEFAULT_POLYPHONY ));
    ui->soundFont->setText( QSTR_SOUNDFONT );
}

void FluidSettingsDialog::showFileDialog()
{
    QDir dir;
#if defined(Q_OS_OSX)
    dir = QDir(QCoreApplication::applicationDirPath() + QLatin1Literal("/../Resources"));
#else
    dir = QDir(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QSTR_DATADIR, QStandardPaths::LocateDirectory));
#endif
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select SoundFont"), dir.absolutePath(), tr("SoundFont Files (*.sf2)"));
    if (!fileName.isEmpty()) {
        ui->soundFont->setText(fileName);
    }
}

void FluidSettingsDialog::changeSoundFont(const QString& fileName)
{
    readSettings();
    ui->soundFont->setText(fileName);
    writeSettings();
}
