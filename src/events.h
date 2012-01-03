/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2012, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#ifndef EVENTS_H
#define EVENTS_H

#include "mididefs.h"
#include <QtCore/QEvent>

const QEvent::Type NoteOffEventType = QEvent::Type(
        QEvent::registerEventType( QEvent::User + STATUS_NOTEOFF ) );
const QEvent::Type NoteOnEventType = QEvent::Type(
        QEvent::registerEventType( QEvent::User + STATUS_NOTEON ) );
const QEvent::Type PolyKeyPressEventType = QEvent::Type(
        QEvent::registerEventType( QEvent::User + STATUS_POLYAFT ) );
const QEvent::Type ControlChangeEventType = QEvent::Type(
        QEvent::registerEventType( QEvent::User + STATUS_CTLCHG ) );
const QEvent::Type ProgramChangeEventType = QEvent::Type(
        QEvent::registerEventType( QEvent::User + STATUS_PROGRAM ) );
const QEvent::Type ChannelKeyPressEventType = QEvent::Type(
        QEvent::registerEventType( QEvent::User + STATUS_CHANAFT ) );
const QEvent::Type PitchWheelEventType = QEvent::Type(
        QEvent::registerEventType( QEvent::User + STATUS_BENDER) );

class NoteEvent : public QEvent
{
public:
    NoteEvent(unsigned char note, unsigned char val, QEvent::Type type)
        : QEvent(type), m_note(note), m_value(val) { }
    unsigned char getNote() const { return m_note; }
    unsigned char getValue() const { return m_value; }
protected:
    unsigned char m_note;
    unsigned char m_value;
};

class NoteOffEvent : public NoteEvent
{
public:
    NoteOffEvent(unsigned char note, unsigned char vel)
        : NoteEvent(note, vel, NoteOffEventType) { }
};

class NoteOnEvent : public NoteEvent
{
public:
    NoteOnEvent(unsigned char note, unsigned char vel)
        : NoteEvent(note, vel, NoteOnEventType) { }
};

class PolyKeyPressEvent : public NoteEvent
{
public:
    PolyKeyPressEvent(unsigned char note, unsigned char value)
        : NoteEvent(note, value, PolyKeyPressEventType) { }
};

class ControlChangeEvent : public QEvent
{
public:
    ControlChangeEvent(unsigned char ctl, unsigned char value)
        : QEvent(ControlChangeEventType), m_ctl(ctl), m_value(value) { }
    unsigned char getController() const { return m_ctl; }
    unsigned char getValue() const { return m_value; }
private:
    unsigned char m_ctl;
    unsigned char m_value;
};

class ValueEvent : public QEvent
{
public:
    ValueEvent(int value, QEvent::Type type)
        : QEvent(type), m_value(value) { }
    int getValue() const { return m_value; }
private:
    int m_value;
};

class ProgramChangeEvent : public ValueEvent
{
public:
    ProgramChangeEvent(unsigned char value)
        : ValueEvent(value, ProgramChangeEventType) { }
};

class ChannelKeyPressEvent : public ValueEvent
{
public:
    ChannelKeyPressEvent(unsigned char value)
        : ValueEvent(value, ChannelKeyPressEventType) { }
};

class PitchWheelEvent : public ValueEvent
{
public:
    PitchWheelEvent(int value)
        : ValueEvent(value, PitchWheelEventType) { }
};

#endif /* EVENTS_H */
