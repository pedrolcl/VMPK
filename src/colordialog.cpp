/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2020, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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
#include "vpianosettings.h"
#include "ui_colordialog.h"

ColorDialog::ColorDialog(bool hiliteOnly, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ColorDialog),
    m_hilite(hiliteOnly),
    m_workingPalette(PianoPalette(PAL_SINGLE))
{
    m_ui->setupUi(this);
    m_ui->paletteNames->addItems(VPianoSettings::instance()->availablePaletteNames(m_hilite));
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
    delete m_ui;
}

void ColorDialog::accept()
{
    VPianoSettings::instance()->updatePalette(m_workingPalette);
    QDialog::accept();
}

void
ColorDialog::onAnyColorWidgetClicked(int i)
{
    ColorWidget *wdg = findChild<ColorWidget*>(QString("widget_%1").arg(i));
    if (wdg != nullptr)
    {
        QColor color = QColorDialog::getColor(m_workingPalette.getColor(i), this);
        if (color.isValid()) {
            wdg->setFillColor(color);
            m_workingPalette.setColor(i, color);
        }
        else
        {
            qWarning() << Q_FUNC_INFO << "invalid color:" << color;
        }
    }
}

void
ColorDialog::refreshPalette()
{
    m_ui->paletteNames->setEditText(m_workingPalette.paletteName());
    m_ui->paletteText->setText(m_workingPalette.paletteText());
    for (int i = 0; i < 16; ++i)
    {
        ColorWidget *wdg = findChild<ColorWidget*>(QString("widget_%1").arg(i));
        if (wdg != nullptr)
        {
            if (i < m_workingPalette.getNumColors()) {
                QColor color = m_workingPalette.getColor(i);
                QString name = m_workingPalette.getColorName(i);
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
ColorDialog::loadPalette(int i)
{
    if (i > -1 && i <  VPianoSettings::instance()->availablePalettes()) {
        m_workingPalette = VPianoSettings::instance()->getPalette(i);
        m_ui->paletteNames->setCurrentIndex(i);
        refreshPalette();
    }
}

void
ColorDialog::resetCurrentPalette()
{
    m_workingPalette.resetColors();
    refreshPalette();
}

void
ColorDialog::retranslateUi()
{
    m_ui->retranslateUi(this);
    m_ui->paletteNames->clear();
    VPianoSettings::instance()->retranslatePalettes();
    m_ui->paletteNames->addItems(VPianoSettings::instance()->availablePaletteNames(m_hilite));
    m_workingPalette.retranslateStrings();
    loadPalette(m_workingPalette.paletteId());
}

int ColorDialog::selectedPalette() const
{
    return m_workingPalette.paletteId();
}
