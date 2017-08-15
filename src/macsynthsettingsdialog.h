/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2017, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#ifndef MacSynthSettingsDialog_H
#define MacSynthSettingsDialog_H

#include <QDialog>
#include <QShowEvent>

namespace Ui {
    class MacSynthSettingsDialog;
}

class MacSynthSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MacSynthSettingsDialog(QWidget *parent = 0);
    ~MacSynthSettingsDialog();
    void readSettings();
    void writeSettings();
    void changeSoundFont(const QString& fileName);

public slots:
    void accept();
    void showEvent(QShowEvent *event);
    void restoreDefaults();
    void showFileDialog();

private:
    Ui::MacSynthSettingsDialog *ui;
};

#endif // MacSynthSettingsDialog_H
