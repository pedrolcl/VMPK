/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2026, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include "eventfilter.h"

#include <QKeyEvent>

EventFilter::EventFilter(QObject *parent)
    : QObject{parent}
{}

bool EventFilter::eventFilter(QObject *watched, QEvent *event)
{
    if (!m_enabled || (m_handler == nullptr)) {
        return QObject::eventFilter(watched, event);
    }

    if (event->type() == QEvent::Type::KeyPress || event->type() == QEvent::Type::KeyRelease) {
        auto keyEvent = static_cast<QKeyEvent *>(event);
        if (!keyEvent->isAutoRepeat()) {
            int keyid =
#if defined(Q_OS_MACOS)
                keyEvent->nativeVirtualKey();
#else
                keyEvent->nativeScanCode();
#endif
            // qDebug() << Q_FUNC_INFO << keyEvent->type() << "key:" << keyid;
            if (keyEvent->type() == QEvent::Type::KeyPress) {
                return m_handler->handleKeyPressed(keyid);
            } else if (keyEvent->type() == QEvent::Type::KeyRelease) {
                return m_handler->handleKeyReleased(keyid);
            }
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}
