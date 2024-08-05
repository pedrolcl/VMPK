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

#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <drumstick/pianopalette.h>

namespace Ui {
    class ColorDialog;
}

using namespace drumstick::widgets;

class ColorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ColorDialog(bool hiliteOnly, QWidget *parent = nullptr);
    ~ColorDialog();
    void retranslateUi();
    int selectedPalette() const;

public slots:
    void loadPalette(int i);
    void accept() override;

private slots:
    void onAnyColorWidgetClicked(int i);
    void resetCurrentPalette();

private:
    void refreshPalette();

    Ui::ColorDialog* m_ui;
    bool m_hilite;
    PianoPalette m_workingPalette;
};

#endif // COLORDIALOG_H
