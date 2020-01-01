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

#ifndef NATIVEFILTER_H
#define NATIVEFILTER_H

#include <QAbstractNativeEventFilter>
#include "pianodefs.h"

class RawKbdHandler {
public:
    virtual ~RawKbdHandler() {}
    virtual bool handleKeyPressed(int keycode) = 0;
    virtual bool handleKeyReleased(int keycode) = 0;
};

class VPIANO_EXPORT NativeFilter : public QAbstractNativeEventFilter
{
public:
    NativeFilter(): m_enabled(false), m_handler(0) {}
    virtual ~NativeFilter() {}

    RawKbdHandler *getRawKbdHandler() { return m_handler; }
    void setRawKbdHandler(RawKbdHandler* h) { m_handler = h; }
    bool isRawKbdEnabled() { return m_enabled; }
    void setRawKbdEnabled(bool b) { m_enabled = b; }

    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *) Q_DECL_OVERRIDE;

private:
    bool m_enabled;
    RawKbdHandler *m_handler;
};

#endif // NATIVEFILTER_H
