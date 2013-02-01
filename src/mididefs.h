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

#ifndef MIDIDEFS_H
#define MIDIDEFS_H

#define MASK_CHANNEL      0x0f
#define MASK_STATUS       0xf0
#define MASK_SAFETY       0x7f

#define CTL_MSB              0
#define CTL_LSB           0x20
#define CTL_ALL_SOUND_OFF 0x78
#define CTL_RESET_ALL_CTL 0x79
#define CTL_ALL_NOTES_OFF 0x7B

#define STATUS_NOTEOFF    0x80
#define STATUS_NOTEON     0x90
#define STATUS_POLYAFT    0xA0
#define STATUS_CTLCHG     0xB0
#define STATUS_PROGRAM    0xC0
#define STATUS_CHANAFT    0xD0
#define STATUS_BENDER     0xE0

#define BENDER_MIN       -8192
#define BENDER_MAX        8191
#define BENDER_MID      0x2000

#define CALC_LSB(x) (x % 0x80)
#define CALC_MSB(x) (x / 0x80)

#define CTL_VOLUME           7
#define CTL_PAN             10
#define CTL_EXPRESSION      11

#endif /* MIDIDEFS_H */
