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

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QApplication>
#include <QPainter>
#include <QDebug>
#include <QFileInfo>

#include "iconutils.h"

namespace IconUtils
{
    void PaintPixmap(QPixmap& pixmap, const QColor& color)
    {
        if (!pixmap.isNull()) {
            QPainter painter(&pixmap);
            painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.fillRect(pixmap.rect(), color);
        }
    }

    QPixmap GetPixmap(const QString& fileName)
    {
        QPixmap pixmap;
        if (QFileInfo::exists(fileName)) {
            QImage image(fileName);
            pixmap = QPixmap::fromImage(image);
            if (image.allGray()) {
                PaintPixmap(pixmap, qApp->palette().color(QPalette::Active, QPalette::WindowText));
            }
        } else {
            qWarning() << "BUG! missing file:" << fileName;
        }
        return pixmap;
    }

    QIcon GetStandardIcon(QStyle::StandardPixmap sp)
    {
        return qApp->style()->standardIcon(sp);
    }

    QIcon GetIcon(const QString &name)
    {
        QIcon icon;
        if (QIcon::hasThemeIcon(name)) {
            icon = QIcon::fromTheme(name);
        }
        if (icon.isNull()) {
            QString iconName = QString(":/vpiano/%1.png").arg(name);
            icon = QIcon(GetPixmap(iconName));
        }
        return icon;
    }

} // namespace IconUtils
