/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2016, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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
                wdg->setColorName(name);
                if (!color.isValid()) {
                    color = qApp->palette().window().color();
                }
                wdg->setFillColor(color);
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
    palette->setColor(0, trUtf8("N"), qApp->palette().highlight().color());
    palette->setColor(1, trUtf8("#"), QColor("lawngreen"));
}

void ColorDialog::resetPaletteChannels(PianoPalette *palette)
{
    palette->setColor(0, trUtf8("1"), QColor("red"));
    palette->setColor(1, trUtf8("2"), QColor("lime"));
    palette->setColor(2, trUtf8("3"), QColor("blue"));
    palette->setColor(3, trUtf8("4"), QColor("gold"));
    palette->setColor(4, trUtf8("5"), QColor("maroon"));
    palette->setColor(5, trUtf8("6"), QColor("green"));
    palette->setColor(6, trUtf8("7"), QColor("navy"));
    palette->setColor(7, trUtf8("8"), QColor("darkorange"));
    palette->setColor(8, trUtf8("9"), QColor("purple"));
    palette->setColor(9, trUtf8("10"), QColor());
    palette->setColor(10, trUtf8("11"), QColor("teal"));
    palette->setColor(11, trUtf8("12"), QColor("chocolate"));
    palette->setColor(12, trUtf8("13"), QColor("fuchsia"));
    palette->setColor(13, trUtf8("14"), QColor("olivedrab"));
    palette->setColor(14, trUtf8("15"), QColor("aqua"));
    palette->setColor(15, trUtf8("16"), QColor("greenyellow"));
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
    palette->setColor(0, trUtf8("C"), QColor::fromRgb(0,0,255));
    palette->setColor(1, trUtf8("C#"), QColor::fromRgb(127,0,255));
    palette->setColor(2, trUtf8("D"), QColor::fromRgb(255,0,255));
    palette->setColor(3, trUtf8("D#"), QColor::fromRgb(255,0,127));
    palette->setColor(4, trUtf8("E"), QColor::fromRgb(255,0,0));
    palette->setColor(5, trUtf8("F"), QColor::fromRgb(255,127,0));
    palette->setColor(6, trUtf8("F#"), QColor::fromRgb(255,255,0));
    palette->setColor(7, trUtf8("G"), QColor::fromRgb(127,255,0));
    palette->setColor(8, trUtf8("G#"), QColor::fromRgb(0,255,0));
    palette->setColor(9, trUtf8("A"), QColor::fromRgb(0,255,127));
    palette->setColor(10, trUtf8("A#"), QColor::fromRgb(0,255,255));
    palette->setColor(11, trUtf8("B"), QColor::fromRgb(0,127,255));
}

void
ColorDialog::retranslatePaletteSingle(PianoPalette *palette)
{
    palette->setColorName(0, QString());
}

void
ColorDialog::retranslatePaletteDouble(PianoPalette *palette)
{
    palette->setColorName(0, trUtf8("N"));
    palette->setColorName(1, trUtf8("#"));
}

void
ColorDialog::retranslatePaletteChannels(PianoPalette *palette)
{
    palette->setColorName(0, trUtf8("1"));
    palette->setColorName(1, trUtf8("2"));
    palette->setColorName(2, trUtf8("3"));
    palette->setColorName(3, trUtf8("4"));
    palette->setColorName(4, trUtf8("5"));
    palette->setColorName(5, trUtf8("6"));
    palette->setColorName(6, trUtf8("7"));
    palette->setColorName(7, trUtf8("8"));
    palette->setColorName(8, trUtf8("9"));
    palette->setColorName(9, trUtf8("10"));
    palette->setColorName(10, trUtf8("11"));
    palette->setColorName(11, trUtf8("12"));
    palette->setColorName(12, trUtf8("13"));
    palette->setColorName(13, trUtf8("14"));
    palette->setColorName(14, trUtf8("15"));
    palette->setColorName(15, trUtf8("16"));
}

void
ColorDialog::retranslatePaletteScale(PianoPalette *palette)
{
    palette->setColorName(0, trUtf8("C"));
    palette->setColorName(1, trUtf8("C#"));
    palette->setColorName(2, trUtf8("D"));
    palette->setColorName(3, trUtf8("D#"));
    palette->setColorName(4, trUtf8("E"));
    palette->setColorName(5, trUtf8("F"));
    palette->setColorName(6, trUtf8("F#"));
    palette->setColorName(7, trUtf8("G"));
    palette->setColorName(8, trUtf8("G#"));
    palette->setColorName(9, trUtf8("A"));
    palette->setColorName(10, trUtf8("A#"));
    palette->setColorName(11, trUtf8("B"));
}

void
ColorDialog::initializePaletteStrings()
{
    PianoPalette *palette;
    palette = m_paletteList[PAL_SINGLE];
    palette->setPaletteName(trUtf8("Single color"));
    palette->setPaletteText(trUtf8("A single color to highlight all note events"));
    retranslatePaletteSingle(palette);

    palette = m_paletteList[PAL_DOUBLE];
    palette->setPaletteName(trUtf8("Two colors"));
    palette->setPaletteText(trUtf8("One color to highlight natural notes and a different one for accidentals"));
    retranslatePaletteDouble(palette);

    palette = m_paletteList[PAL_CHANNELS];
    palette->setPaletteName(trUtf8("MIDI Channels"));
    palette->setPaletteText(trUtf8("A different color for each MIDI channel. Enable Omni mode in the MIDI IN connection"));
    retranslatePaletteChannels(palette);

    palette = m_paletteList[PAL_SCALE];
    palette->setPaletteName(trUtf8("Chromatic scale"));
    palette->setPaletteText(trUtf8("One color for each note in the chromatic scale"));
    retranslatePaletteScale(palette);
}

void
ColorDialog::initializePalettes()
{
    PianoPalette *palette;

    palette = new PianoPalette(1, PAL_SINGLE);
    m_paletteList.insert(PAL_SINGLE, palette);

    palette = new PianoPalette(2, PAL_DOUBLE);
    resetPaletteDouble(palette);
    m_paletteList.insert(PAL_DOUBLE, palette);

    palette = new PianoPalette(16, PAL_CHANNELS);
    resetPaletteChannels(palette);
    m_paletteList.insert(PAL_CHANNELS, palette);

    palette = new PianoPalette(12, PAL_SCALE);
    resetPaletteScale(palette);
    m_paletteList.insert(PAL_SCALE, palette);

    initializePaletteStrings();
}

PianoPalette *
ColorDialog::currentPalette()
{
    if (m_currentPalette == 0) {
        m_currentPalette = m_paletteList[0];
    }
    return m_currentPalette;
}

PianoPalette *
ColorDialog::getPalette(int pal)
{
    if (pal >= 0 && pal < m_paletteList.count()) {
        return m_paletteList[pal];
    }
    return 0;
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

void
ColorDialog::retranslateUi()
{
    m_ui->retranslateUi(this);
    initializePaletteStrings();
    m_ui->paletteNames->clear();
    m_ui->paletteNames->addItems(availablePaletteNames());
    loadPalette(currentPalette()->paletteId());
}
