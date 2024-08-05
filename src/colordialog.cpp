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

#include <QColorDialog>
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
    for(int i=0; i<VPianoSettings::instance()->availablePalettes(); ++i) {
        PianoPalette p = VPianoSettings::instance()->getPalette(i);
        if (m_hilite && !p.isHighLight()) {
            continue;
        }
        m_ui->paletteNames->addItem(p.paletteName(), p.paletteId());
    }
    for(int i = 0; i < 16; ++i)
    {
        ColorWidget *wdg = new ColorWidget(m_ui->groupBox);
        wdg->setObjectName(QString("widget_%1").arg(i));
        wdg->disable();
        m_ui->gridLayout->addWidget(wdg, i / 4, i % 4, 1, 1);
        connect(wdg, &ColorWidget::clicked, this, [=]{ onAnyColorWidgetClicked(i); });
    }
    connect(m_ui->paletteNames, QOverload<int>::of(&QComboBox::activated), this, [=](int index){ loadPalette(m_ui->paletteNames->itemData(index).toInt()); });
    QPushButton *btnRestoreDefs = m_ui->buttonBox->button(QDialogButtonBox::RestoreDefaults);
    connect(btnRestoreDefs, &QAbstractButton::clicked, this, &ColorDialog::resetCurrentPalette);
}

ColorDialog::~ColorDialog()
{
    delete m_ui;
}

void
ColorDialog::accept()
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
    if (i >= PAL_SINGLE && i <= PAL_HISCALE) {
        m_workingPalette = VPianoSettings::instance()->getPalette(i);
        m_ui->paletteNames->setCurrentText(m_workingPalette.paletteName());
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

int
ColorDialog::selectedPalette() const
{
    return m_workingPalette.paletteId();
}
