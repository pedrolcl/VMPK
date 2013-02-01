/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2013, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDesktopServices>

#include "riffimportdlg.h"
#include "ui_riffimportdlg.h"

RiffImportDlg::RiffImportDlg(QWidget *parent)
    : QDialog(parent), ui(new Ui::RiffImportDlg)
{
    m_riff = new Riff(this);
    connect(m_riff, SIGNAL(signalInstrument(int, int, QString)), SLOT(slotInstrument(int, int, QString)));
    connect(m_riff, SIGNAL(signalSoundFont(QString, QString, QString)), SLOT(slotCompleted(QString, QString, QString)));
    connect(m_riff, SIGNAL(signalDLS(QString, QString, QString)), SLOT(slotCompleted(QString, QString, QString)));

    ui->setupUi(this);
    connect(ui->m_inputBtn, SIGNAL(clicked()), SLOT(openInput()));
    connect(ui->m_outputBtn, SIGNAL(clicked()), SLOT(openOutput()));
}

RiffImportDlg::~RiffImportDlg()
{
    delete ui;
}

void RiffImportDlg::openInput()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Input SoundFont"), QString::null, tr("SoundFonts (*.sf2 *.sbk *.dls)"));
    if (!fileName.isEmpty()) {
        setInput(fileName);
        m_riff->readFromFile(m_input);
    }
}

void RiffImportDlg::openOutput()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Output"), m_output, tr("Instrument definitions (*.ins)"));
    if (!fileName.isEmpty())
        setOutput(fileName);
}

void RiffImportDlg::setInput(QString fileName)
{
    QFileInfo f(fileName);
    if (f.exists()) {
        ui->m_input->setText(m_input = fileName);
#if QT_VERSION < 0x040400
        QDir dir = QDir::home();
#else
        //QDir dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        QDir dir = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#endif
        QString fullFilespec = dir.absoluteFilePath(f.baseName() + ".ins");
        setOutput(fullFilespec);
    }
}

void RiffImportDlg::setOutput(QString fileName)
{
    ui->m_output->setText(m_output = fileName);
}

void RiffImportDlg::slotInstrument(int bank, int pc, QString name)
{
    m_ins[bank][pc] = name;
}

void RiffImportDlg::slotCompleted(QString name, QString version, QString copyright)
{
    ui->m_name->setText(m_name = name);
    ui->m_version->setText(version);
    ui->m_copyright->setText(copyright);
}

void RiffImportDlg::save()
{
    QFile data(m_output);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&data);
        out << "; Name: " << m_name << endl;
        out << "; Version: " << ui->m_version->text() << endl;
        out << "; Copyright: " << ui->m_copyright->text() << endl << endl;
        out << endl << ".Patch Names" << endl;
        QMapIterator<int,Bank> i(m_ins);
        while( i.hasNext() ) {
            i.next();
            out << endl << "[Bank" << i.key() << "]" << endl;
            Bank b = i.value();
            QMapIterator<int,QString> j(b);
            while( j.hasNext() ) {
                j.next();
                out << j.key() << "=" << j.value() << endl;
            }
        }
        out << endl << ".Controller Names" << endl;
        out << endl << "[Standard]" << endl;
        out << "1=1-Modulation" << endl;
        out << "2=2-Breath" << endl;
        out << "4=4-Foot controller" << endl;
        out << "5=5-Portamento time" << endl;
        out << "7=7-Volume" << endl;
        out << "8=8-Balance" << endl;
        out << "10=10-Pan" << endl;
        out << "11=11-Expression" << endl;
        out << "64=64-Pedal (sustain)" << endl;
        out << "65=65-Portamento" << endl;
        out << "66=66-Pedal (sostenuto)" << endl;
        out << "67=67-Pedal (soft)" << endl;
        out << "69=69-Hold 2" << endl;
        out << "91=91-External Effects depth" << endl;
        out << "92=92-Tremolo depth" << endl;
        out << "93=93-Chorus depth" << endl;
        out << "94=94-Celeste (detune) depth" << endl;
        out << "95=95-Phaser depth" << endl;
        out << endl << ".Instrument Definitions" << endl;
        out << endl << "["  << m_name <<  "]" << endl;
        out << "Control=Standard" << endl;
        i.toFront();
        while( i.hasNext() ) {
            i.next();
            out << "Patch[" << i.key() << "]=Bank" << i.key() << endl;
        }
        data.close();
    }
}

void RiffImportDlg::retranslateUi()
{
    ui->retranslateUi(this);
}
