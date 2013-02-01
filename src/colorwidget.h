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

#ifndef COLORWIDGET_H
#define COLORWIDGET_H

#include <QWidget>

class ColorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColorWidget(QWidget *parent = 0);

    QString colorName() const { return m_colorName; }
    void setColorName(const QString value);

    QColor fillColor() const { return m_fillColor; }
    void setFillColor(const QColor value);

    void disable();

    void mousePressEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

signals:
    void clicked();

private:
    QString m_colorName;
    QColor m_fillColor;

};

#endif // COLORWIDGET_H
