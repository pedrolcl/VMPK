/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2012, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <QColorDialog>
#include <QSignalMapper>
#include <QPushButton>
#include <QDebug>

#include "constants.h"
#include "colorwidget.h"
#include "colordialog.h"
#include "ui_colordialog.h"

ColorDialog::ColorDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ColorDialog), m_currentPalette(0)
{
    m_ui->setupUi(this);
    initializePalettes();
    m_ui->paletteNames->addItems(availablePaletteNames());
    QSignalMapper *signalMapper = new QSignalMapper(this);
    for(int i = 0; i < 16; ++i)
    {
        ColorWidget *wdg = new ColorWidget(m_ui->groupBox);
        wdg->setObjectName(QString("widget_%1").arg(i));
        wdg->disable();
        signalMapper->setMapping(wdg, i);
        connect(wdg, SIGNAL(clicked()), signalMapper, SLOT(map()));
        m_ui->gridLayout->addWidget(wdg, i / 4, i % 4, 1, 1);
    }
    connect(signalMapper, SIGNAL(mapped(int)), SLOT(onAnyColorWidgetClicked(int)));
    connect(m_ui->paletteNames, SIGNAL(activated(int)), SLOT(loadPalette(int)));
    QPushButton *btnRestoreDefs = m_ui->buttonBox->button(QDialogButtonBox::RestoreDefaults);
    connect(btnRestoreDefs, SIGNAL(clicked()), SLOT(resetCurrentPalette()));
}

ColorDialog::~ColorDialog()
{
    foreach(PianoPalette *p, m_paletteList) {
        delete p;
    }
    m_paletteList.clear();
    delete m_ui;
}

void
ColorDialog::onAnyColorWidgetClicked(int i)
{
    ColorWidget *wdg = findChild<ColorWidget*>(QString("widget_%1").arg(i));
    if (wdg != 0)
    {
        QColor color = QColorDialog::getColor(currentPalette()->getColor(i), this);
        if (color.isValid()) {
            wdg->setFillColor(color);
            currentPalette()->setColor(i, color);
        }
    }
}

QColor
ColorDialog::getColor(int i)
{
    return currentPalette()->getColor(i);
}

void
ColorDialog::refreshPalette()
{
    m_ui->paletteNames->setEditText(currentPalette()->paletteName());
    m_ui->paletteText->setText(currentPalette()->paletteText());
    for(int i = 0; i < 16; ++i)
    {
        ColorWidget *wdg = findChild<ColorWidget*>(QString("widget_%1").arg(i));
        if (wdg != 0)
        {
            if (i < currentPalette()->getNumColors()) {
                QColor color = currentPalette()->getColor(i);
                QString name = currentPalette()->getColorName(i);
                if (color.isValid()) {
                    wdg->setFillColor(color);
                    wdg->setColorName(name);
                }
            } else {
                wdg->disable();
            }
        }
    }
}

void
ColorDialog::saveCurrentPalette()
{
    currentPalette()->saveColors();
}

void
ColorDialog::loadPalette(int i)
{
    if (i > -1 && i < m_paletteList.length()) {
        PianoPalette *p = m_paletteList[i];
        m_currentPalette = p;
        m_currentPalette->loadColors();
        m_ui->paletteNames->setCurrentIndex(i);
        refreshPalette();
    }
}

void ColorDialog::resetPaletteSingle(PianoPalette *palette)
{
    palette->setColor(0, QString(), qApp->palette().highlight().color());
}

void ColorDialog::resetPaletteDouble(PianoPalette *palette)
{
    palette->setColor(0, tr("N"), qApp->palette().highlight().color());
    palette->setColor(1, tr("#"), QColor("lawngreen"));
}

void ColorDialog::resetPaletteChannels(PianoPalette *palette)
{
    palette->setColor(0, "1", QColor("blue"));
    palette->setColor(1, "2", QColor("green"));
    palette->setColor(2, "3", QColor("yellow"));
    palette->setColor(3, "4", QColor("orange"));
    palette->setColor(4, "5", QColor("chocolate"));
    palette->setColor(5, "6", QColor("fuchsia"));
    palette->setColor(6, "7", QColor("cyan"));
    palette->setColor(7, "8", QColor("darkgreen"));
    palette->setColor(8, "9", QColor("lime"));
    palette->setColor(9, "10", QColor("red"));
    palette->setColor(10, "11", QColor("darkblue"));
    palette->setColor(11, "12", QColor("olive"));
    palette->setColor(12, "13", QColor("gold"));
    palette->setColor(13, "14", QColor("maroon"));
    palette->setColor(14, "15", QColor("crimson"));
    palette->setColor(15, "16", QColor("purple"));
}

void ColorDialog::resetPaletteScale(PianoPalette *palette)
{
    /*
                    R       G       B
            C       -       -       100%    0
            C#      50%     -       100%    1
            D       100%    -       100%    2
            D#      100%    -       50%     3
            E       100%    -       -       4
            F       100%    50%     -       5
            F#      100%    100%    -       6
            G       50%     100%    -       7
            G#      -       100%    -       8
            A       -       100%    50%     9
            A#      -       100%    100%    10
            B       -       50%     100%    11
    */
    palette->setColor(0, tr("C"), QColor::fromRgb(0,0,255));
    palette->setColor(1, tr("C#"), QColor::fromRgb(127,0,255));
    palette->setColor(2, tr("D"), QColor::fromRgb(255,0,255));
    palette->setColor(3, tr("D#"), QColor::fromRgb(255,0,127));
    palette->setColor(4, tr("E"), QColor::fromRgb(255,0,0));
    palette->setColor(5, tr("F"), QColor::fromRgb(255,127,0));
    palette->setColor(6, tr("F#"), QColor::fromRgb(255,255,0));
    palette->setColor(7, tr("G"), QColor::fromRgb(127,255,0));
    palette->setColor(8, tr("G#"), QColor::fromRgb(0,255,0));
    palette->setColor(9, tr("A"), QColor::fromRgb(0,255,127));
    palette->setColor(10, tr("A#"), QColor::fromRgb(0,255,255));
    palette->setColor(11, tr("B"), QColor::fromRgb(0,127,255));
}

void
ColorDialog::initializePalettes()
{
    PianoPalette *palette;

    palette = new PianoPalette(1, PAL_SINGLE);
    palette->setPaletteName(tr("Single color"));
    palette->setPaletteText(tr("A single color to highlight all note events"));
    insertPalette(PAL_SINGLE, palette);

    palette = new PianoPalette(2, PAL_DOUBLE);
    palette->setPaletteName(tr("Two colors"));
    palette->setPaletteText(tr("One color to highlight natural notes and a different one for alterations"));
    resetPaletteDouble(palette);
    insertPalette(PAL_DOUBLE, palette);

    palette = new PianoPalette(16, PAL_CHANNELS);
    palette->setPaletteName(tr("MIDI Channels"));
    palette->setPaletteText(tr("A different color for each MIDI channel. Enable Omni mode in the MIDI IN connection"));
    resetPaletteChannels(palette);
    insertPalette(PAL_CHANNELS, palette);

    palette = new PianoPalette(12, PAL_SCALE);
    palette->setPaletteName(tr("Chromatic scale"));
    palette->setPaletteText(tr("One color for each note in the chromatic scale"));
    resetPaletteScale(palette);
    insertPalette(PAL_SCALE, palette);
}

void
ColorDialog::insertPalette(const int i, PianoPalette *p)
{
    m_paletteList.insert(i, p);
}

PianoPalette *
ColorDialog::currentPalette()
{
    if (m_currentPalette == 0) {
        m_currentPalette = m_paletteList[0];
    }
    return m_currentPalette;
}

QList<QString>
ColorDialog::availablePaletteNames()
{
    QList<QString> tmp;
    foreach(PianoPalette *p, m_paletteList) {
        tmp << p->paletteName();
    }
    return tmp;
}

void
ColorDialog::resetCurrentPalette()
{
    PianoPalette *palette = currentPalette();
    switch(palette->paletteId()) {
    case PAL_SINGLE:
        resetPaletteSingle(palette);
        break;
    case PAL_DOUBLE:
        resetPaletteDouble(palette);
        break;
    case PAL_CHANNELS:
        resetPaletteChannels(palette);
        break;
    case PAL_SCALE:
        resetPaletteScale(palette);
        break;
    default:
        return;
    }
    refreshPalette();
}
