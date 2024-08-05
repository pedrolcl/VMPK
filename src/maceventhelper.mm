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

#include "maceventhelper.h"
#include <Cocoa/Cocoa.h>

class MacEventHelper::Private
{
public:
   NSEvent *m_event;
};

MacEventHelper::MacEventHelper()
{
    d = new Private();
}

MacEventHelper::~MacEventHelper()
{
    delete d;
}

void
MacEventHelper::setNativeEvent(void *p)
{
    d->m_event = static_cast<NSEvent *>(p);
}

void
MacEventHelper::clearNativeEvent()
{
    d->m_event = nullptr;
}

bool
MacEventHelper::isKeyPress()
{
    return (d->m_event != nullptr) && ([d->m_event type] == NSEventType::NSEventTypeKeyDown);
}

bool
MacEventHelper::isKeyRelease()
{
    return (d->m_event != nullptr) && ([d->m_event type] == NSEventType::NSEventTypeKeyUp);
}

bool
MacEventHelper::isKeyEvent()
{
    return (d->m_event != nullptr) && (([d->m_event type] == NSEventType::NSEventTypeKeyDown) || ([d->m_event type] == NSEventType::NSEventTypeKeyUp));
}

bool
MacEventHelper::isNotRepeated()
{
    return (d->m_event != nullptr) && ([d->m_event isARepeat] == NO);
}

int
MacEventHelper::rawKeyCode()
{
    if (d->m_event != nullptr)
       return [d->m_event keyCode];
    return -1;
}

