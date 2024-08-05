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

#ifndef NATIVEFILTER_H
#define NATIVEFILTER_H

#include <QAbstractNativeEventFilter>
#include <drumstick/pianokeybd.h>

using namespace drumstick::widgets;

class NativeFilter : public QAbstractNativeEventFilter
{
public:
    NativeFilter(): m_enabled(false), m_handler(nullptr) {}
    virtual ~NativeFilter() {}

    RawKbdHandler *getRawKbdHandler() { return m_handler; }
    void setRawKbdHandler(RawKbdHandler* h) { m_handler = h; }
    bool isRawKbdEnabled() { return m_enabled; }
    void setRawKbdEnabled(bool b) { m_enabled = b; }

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *) override;
#else
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr*) override;
#endif

private:
    bool m_enabled;
    RawKbdHandler *m_handler;
};

#endif // NATIVEFILTER_H
