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

#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include "pianopalette.h"

namespace Ui {
    class ColorDialog;
}

class ColorDialog : public QDialog
{
    Q_OBJECT
    
public:

    explicit ColorDialog(QWidget *parent = 0);
    ~ColorDialog();
    PianoPalette *currentPalette();
    PianoPalette *getPalette(int pal);
    QList<QString> availablePaletteNames();
    QColor getColor(int i);
    void saveCurrentPalette();
    void retranslateUi();

public slots:
    void loadPalette(int i);

private slots:
    void onAnyColorWidgetClicked(int i);
    void resetCurrentPalette();

private:
    void initializePaletteStrings();
    void initializePalettes();
    void resetPaletteSingle(PianoPalette *palette);
    void resetPaletteDouble(PianoPalette *palette);
    void resetPaletteChannels(PianoPalette *palette);
    void resetPaletteScale(PianoPalette *palette);
    void refreshPalette();

    Ui::ColorDialog *m_ui;
    QList<PianoPalette *> m_paletteList;
    PianoPalette *m_currentPalette;
};

#endif // COLORDIALOG_H
