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

#ifndef KMAPDIALOG_H
#define KMAPDIALOG_H

#include <QDialog>
#include "ui_kmapdialog.h"
#include "keyboardmap.h"

class KMapDialog : public QDialog
{
    Q_OBJECT

public:
    KMapDialog(QWidget *parent = nullptr);
    void load(const QString fileName);
    void save(const QString fileName);
    void displayMap(const VMPKKeyboardMap* map);
    void getMap(VMPKKeyboardMap* map);
    void retranslateUi();

public slots:
    void slotOpen();
    void slotSave();

private:
    void updateMap();

    QPushButton* m_btnOpen;
    QPushButton* m_btnSave;
    VMPKKeyboardMap m_map;
    Ui::KMapDialogClass ui;
};

#endif // KMAPDIALOG_H
