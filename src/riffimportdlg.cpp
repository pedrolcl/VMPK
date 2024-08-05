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

#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDesktopServices>
#include <QStandardPaths>

#include "iconutils.h"
#include "riffimportdlg.h"
#include "ui_riffimportdlg.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
#define endl Qt::endl
#endif

RiffImportDlg::RiffImportDlg(QWidget *parent)
    : QDialog(parent), ui(new Ui::RiffImportDlg)
{
    m_riff = new Riff(this);
    connect(m_riff, &Riff::signalInstrument, this, &RiffImportDlg::slotInstrument);
    connect(m_riff, &Riff::signalPercussion, this, &RiffImportDlg::slotPercussion);
    connect(m_riff, &Riff::signalSoundFont, this, &RiffImportDlg::slotCompleted);
    connect(m_riff, &Riff::signalDLS, this, &RiffImportDlg::slotCompleted);

    ui->setupUi(this);
    ui->m_inputBtn->setIcon(IconUtils::GetIcon("wrench"));
    ui->m_outputBtn->setIcon(IconUtils::GetIcon("wrench"));
    connect(ui->m_inputBtn, &QToolButton::clicked, this, &RiffImportDlg::openInput);
    connect(ui->m_outputBtn, &QToolButton::clicked, this, &RiffImportDlg::openOutput);
}

RiffImportDlg::~RiffImportDlg()
{
    delete ui;
}

void RiffImportDlg::openInput()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Input SoundFont"), QString(), tr("SoundFonts (*.sf2 *.sbk *.dls)"));
    if (!fileName.isEmpty()) {
        setInput(fileName);
        m_riff->readFromFile(m_input);
    }
}

void RiffImportDlg::openOutput()
{
    QFileDialog dlg(this);
    dlg.setNameFilter(tr("Instrument definitions (*.ins)"));
    dlg.setWindowTitle(tr("Output"));
    dlg.setDefaultSuffix("ins");
    dlg.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    dlg.selectFile(m_output);
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    if (dlg.exec() == QFileDialog::Accepted) {
        const QString fileName = dlg.selectedFiles().constFirst();
        setOutput(fileName);
    }
}

void RiffImportDlg::setInput(QString fileName)
{
    QFileInfo f(fileName);
    if (f.exists()) {
        ui->m_input->setText(m_input = fileName);
        QDir dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
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
    //qDebug() << Q_FUNC_INFO << bank << pc << name;
    m_ins[bank][pc] = name;
}

void RiffImportDlg::slotPercussion(int bank, int pc, QString name)
{
    //qDebug() << Q_FUNC_INFO << bank << pc << name;
    m_perc[bank][pc] = name;
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
        QMapIterator<int,Bank> im(m_ins);
        while( im.hasNext() ) {
            im.next();
            out << endl << "[Bank" << im.key() << "]" << endl;
            Bank b = im.value();
            QMapIterator<int,QString> j(b);
            while( j.hasNext() ) {
                j.next();
                out << j.key() << "=" << j.value() << endl;
            }
        }
        QMapIterator<int,Bank> id(m_perc);
        while( id.hasNext() ) {
            id.next();
            out << endl << "[Drums" << id.key() << "]" << endl;
            Bank b = id.value();
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
        out << "BankSelMethod=0" << endl;
        im.toFront();
        while( im.hasNext() ) {
            im.next();
            out << "Patch[" << im.key() << "]=Bank" << im.key() << endl;
        }
        out << endl << "["  << m_name <<  " Drums]" << endl;
        out << "Control=Standard" << endl;
        out << "BankSelMethod=0" << endl;
        out << "Key[*,*]=0..127" << endl;
        out << "Drum[*,*]=1" << endl;
        id.toFront();
        while( id.hasNext() ) {
            id.next();
            out << "Patch[" << id.key() << "]=Drums" << id.key() << endl;
        }
        data.close();
    }
}

void RiffImportDlg::retranslateUi()
{
    ui->retranslateUi(this);
}
