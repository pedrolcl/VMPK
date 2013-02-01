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

#ifndef KEYBOARDMAP_H
#define KEYBOARDMAP_H

#include "constants.h"
#include <QHash>
#include <QIODevice>

class KeyboardMap : public QHash<int, int>
{
    Q_DECLARE_TR_FUNCTIONS(KeyboardMap)
public:
    KeyboardMap() : QHash<int, int>(), m_fileName(QSTR_DEFAULT), m_rawMode(false) {}
    void loadFromXMLFile(const QString fileName);
    void saveToXMLFile(const QString fileName);
    void initializeFromXML(QIODevice *dev);
	void serializeToXML(QIODevice *dev);
	void copyFrom(const KeyboardMap* other);

	void setFileName(const QString fileName) { m_fileName = fileName; }
	const QString& getFileName() const { return m_fileName; }
	void setRawMode(bool b) { m_rawMode = b; }
	bool getRawMode() const { return m_rawMode; }

private:
    void reportError( const QString filename, const QString title, const QString err );
    QString m_fileName;
    bool m_rawMode;
};

#endif /* KEYBOARDMAP_H */
