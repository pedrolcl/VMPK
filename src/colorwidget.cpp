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

#include <QPainter>
#include <QMouseEvent>
#include "colorwidget.h"

ColorWidget::ColorWidget(QWidget *parent) :
    QWidget(parent)
{
}

void ColorWidget::setColorName(const QString value)
{
    if (m_colorName != value)
    {
        m_colorName = value;
        repaint();
    }
}

void ColorWidget::setFillColor(const QColor value)
{
    if (m_fillColor != value && value.isValid())
    {
        m_fillColor = value;
        setEnabled(true);
        repaint();
    }
}

void ColorWidget::paintEvent(QPaintEvent *ev)
{
    if (m_fillColor.isValid())
    {
        QPainter painter(this);
        painter.fillRect(rect(), m_fillColor);
        if (!m_colorName.isEmpty()) {
            QRect maxRect = painter.fontMetrics().boundingRect(m_colorName);
            int bsize = painter.fontMetrics().xHeight()*3;
            int w = maxRect.width() > bsize ? maxRect.width() : bsize;
            int h = maxRect.height() > bsize ? maxRect.height() : bsize;
            QRect lblRect((rect().width() - w) / 2, (rect().height() - h) / 2, w, h);
            painter.fillRect(lblRect, Qt::black);
            painter.setPen(Qt::white);
            painter.drawText(lblRect, Qt::AlignCenter, m_colorName);
        }
        return;
    }
    QWidget::paintEvent(ev);
}

void ColorWidget::mousePressEvent(QMouseEvent *e)
{
    emit clicked();
    e->accept();
}

void ColorWidget::disable()
{
    m_colorName = QString();
    m_fillColor = QColor();
    setEnabled(false);
}
