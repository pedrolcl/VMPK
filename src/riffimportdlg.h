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

#ifndef RIFFIMPORTDLG_H
#define RIFFIMPORTDLG_H

#include <QDialog>
#include <QMap>

#include "riff.h"

namespace Ui
{
    class RiffImportDlg;
}

typedef QMap<int, QString> Bank;

class RiffImportDlg : public QDialog
{
    Q_OBJECT

public:
    RiffImportDlg(QWidget *parent = nullptr);
    ~RiffImportDlg();

    void setInput(QString fileName);
    void setOutput(QString fileName);
    QString getOutput() { return m_output; }
    QString getInput() { return m_input; }
    QString getName() { return m_name; }
    void save();
    void retranslateUi();

protected slots:
    void slotInstrument(int bank, int pc, QString name);
    void slotPercussion(int bank, int pc, QString name);
    void slotCompleted(QString name, QString version, QString copyright);
    void openInput();
    void openOutput();

private:
    Ui::RiffImportDlg *ui;
    Riff* m_riff;
    QMap<int, Bank> m_ins;
    QMap<int, Bank> m_perc;
    QString m_input;
    QString m_output;
    QString m_name;
};

#endif // RIFFIMPORTDLG_H
