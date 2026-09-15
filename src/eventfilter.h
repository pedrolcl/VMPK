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

#ifndef EVENTFILTER_H
#define EVENTFILTER_H

#include <QObject>
#include <drumstick/pianokeybd.h>

using namespace drumstick::widgets;

class EventFilter : public QObject
{
    Q_OBJECT
public:
    explicit EventFilter(QObject *parent = nullptr);

    RawKbdHandler *getRawKbdHandler() { return m_handler; }
    void setRawKbdHandler(RawKbdHandler *h) { m_handler = h; }
    bool isRawKbdEnabled() { return m_enabled; }
    void setRawKbdEnabled(bool b) { m_enabled = b; }

    // QObject interface
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    bool m_enabled{false};
    RawKbdHandler *m_handler{nullptr};
};

#endif // EVENTFILTER_H
