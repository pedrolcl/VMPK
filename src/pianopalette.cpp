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

#include "constants.h"
#include "pianopalette.h"
#include <QApplication>
#include <QPalette>

PianoPalette::PianoPalette(int maxcolors, int id) :
    m_paletteId(id)
{
    for(int i=0; i<maxcolors; ++i) {
        m_colors.insert(i, qApp->palette().highlight().color());
        m_names.insert(i, QString());
    }
}

void
PianoPalette::setColor(int n, QString s, QColor c)
{
    if (n < m_colors.size()) {
        m_colors[n] = c;
        m_names[n] = s;
    }
}

void
PianoPalette::setColor(int n, QColor c)
{
    if (n < m_colors.size())
        m_colors[n] = c;
}

void
PianoPalette::setColorName(int n, QString s)
{
    if (n < m_names.size())
        m_names[n] = s;
}

QColor
PianoPalette::getColor(int i)
{
    if (i < m_colors.size())
        return m_colors[i];
    return QColor();
}

QString
PianoPalette::getColorName(int i)
{
    if (i < m_names.size())
        return m_names[i];
    return QString();
}

int
PianoPalette::getNumColors()
{
    return m_colors.size();
}

void
PianoPalette::setPaletteName(const QString name)
{
    if (m_paletteName != name) {
        m_paletteName = name;
    }
}

void
PianoPalette::setPaletteText(const QString help)
{
    m_paletteText = help;
}

void
PianoPalette::saveColors()
{
    QSettings settings;
    settings.beginWriteArray(QSTR_PALETTEPREFIX + QString::number(m_paletteId));
    for(int i=0; i<m_colors.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("color", m_colors[i]);
    }
    settings.endArray();
}

void
PianoPalette::loadColors()
{
    QSettings settings;
    int size = settings.beginReadArray(QSTR_PALETTEPREFIX + QString::number(m_paletteId));
    if (size > m_colors.size())
        size = m_colors.size();
    for(int i=0; i<size; ++i) {
        settings.setArrayIndex(i);
        QColor c = settings.value("color", QColor()).value<QColor>();
        setColor(i, c);
    }
    settings.endArray();
}
