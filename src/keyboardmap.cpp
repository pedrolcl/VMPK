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

#include "keyboardmap.h"

#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QKeySequence>
#include <QMessageBox>

void KeyboardMap::loadFromXMLFile(const QString fileName)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly | QFile::Text)) {
        initializeFromXML(&f);
        f.close();
        m_fileName = fileName;
        qDebug() << "Loaded Map: " << fileName;
    }
    if (f.error() != QFile::NoError) {
        reportError(fileName, tr("Error loading a file"), f.errorString());
    }
}

void KeyboardMap::saveToXMLFile(const QString fileName)
{
    QFile f(fileName);
    if (f.open(QFile::WriteOnly | QFile::Text)) {
        serializeToXML(&f);
        f.close();
        m_fileName = fileName;
        qDebug() << "Saved Map: " << fileName;
    }
    if (f.error() != QFile::NoError) {
        reportError(fileName, tr("Error saving a file"), f.errorString());
    }
}

void KeyboardMap::initializeFromXML(QIODevice *dev)
{
    QXmlStreamReader reader(dev);
    clear();
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            if (reader.name() == (m_rawMode?"rawkeymap":"keyboardmap")) {
                reader.readNext();
                while (reader.isWhitespace() || reader.isComment()) {
                    //qDebug() << "1st. junk place" << reader.text();
                    reader.readNext();
                }
                while (reader.isStartElement()) {
                    if (reader.name() == "mapping") {
                        QString key = reader.attributes().value(m_rawMode?"keycode":"key").toString();
                        QString sn = reader.attributes().value("note").toString();
                        bool ok = false;
                        int note = sn.toInt(&ok);
                        if (ok) {
                            if (m_rawMode) {
                                int keycode = key.toInt(&ok);
                                if (ok) insert(keycode, note);
                            } else {
                                QKeySequence ks(key);
                                insert(ks[0], note);
                            }
                        }
                    }
                    reader.readNext();
                    while (reader.isWhitespace() || reader.isEndElement() || reader.isComment()) {
                        //qDebug() << "2nd. junk place" << reader.text();
                        reader.readNext();
                    }
                }
            } else {
                reader.readNext();
            }
        }
    }
    if (reader.hasError()) {
        reportError(QString(), tr("Error reading XML"), reader.errorString() );
    }
}

void KeyboardMap::serializeToXML(QIODevice *dev)
{
    QXmlStreamWriter writer(dev);
    writer.setAutoFormatting(true);
    //writer.setCodec("UTF-8");
    writer.writeStartDocument();
    writer.writeDTD(m_rawMode?"<!DOCTYPE rawkeyboardmap>":"<!DOCTYPE keyboardmap>");
    writer.writeStartElement(m_rawMode ? "rawkeymap" : "keyboardmap");
    writer.writeAttribute("version", "1.0");
    foreach(int key, keys()) {
        writer.writeEmptyElement("mapping");
        if (m_rawMode)
            writer.writeAttribute("keycode", QString::number(key));
        else {
            QKeySequence ks(key);
            writer.writeAttribute("key", ks.toString(QKeySequence::PortableText));
        }
        writer.writeAttribute("note", QString::number(value(key)));
    }
    writer.writeEndElement();
    writer.writeEndDocument();
}

void KeyboardMap::copyFrom(const KeyboardMap* other)
{
    m_fileName = other->getFileName();
    m_rawMode = other->getRawMode();
    clear();
    KeyboardMap::ConstIterator it;
    for(it = other->begin(); it != other->end(); ++it)
        insert(it.key(), it.value());
}

void KeyboardMap::reportError( const QString filename,
                               const QString title,
                               const QString err )
{
    QMessageBox::warning(0, title, tr("File: %1\n%2").arg(filename).arg(err));
}
