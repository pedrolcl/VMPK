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

#include <QFile>
#include <QDebug>

#include "riff.h"

Riff::Riff(QObject* parent) :
    QObject(parent)
{}

Riff::~Riff()
{}

#if 0
static QString toString(quint32 ckid)
{
    QByteArray data((char*) &ckid, 4);
    return QString(data);
}
#endif

void Riff::readFromFile(QString fileName)
{
    QFile file(m_fileName = fileName);
    file.open(QIODevice::ReadOnly);
    QDataStream ds(&file);
    readFromStream(&ds);
    file.close();
}

void Riff::readFromStream(QDataStream* ds)
{
    if (ds != NULL) {
        m_IOStream = ds;
        m_IOStream->setByteOrder(QDataStream::LittleEndian);
        read();
    }
}

quint16 Riff::read16bit()
{
    quint16 wrk;
    *m_IOStream >> wrk;
    return wrk;
}

quint32 Riff::read32bit()
{
    quint32 wrk;
    *m_IOStream >> wrk;
    return wrk;
}

QString Riff::readstr(int size)
{
#if defined(Q_CC_GNU) || defined(Q_CC_GCCE)
    char buffer[size+1];
#elif defined(Q_CC_MSVC)
    char *buffer = (char *) alloca (size+1);
#endif
    m_IOStream->readRawData(buffer, size);
    buffer[size] = 0;
    return QString(buffer).trimmed();
}

void Riff::skip(int size)
{
    if (size & 1) size++;
    m_IOStream->skipRawData(size);
}

quint32 Riff::readExpectedChunk(quint32 cktype)
{
    quint32 chunkType, len = 0;
    chunkType = read32bit();
    if (chunkType == cktype) {
        len = read32bit();
//        qDebug() << "Expected chunkType:" << toString(chunkType)
//                 << "(" << hex << chunkType << ")"
//                 << "length:" << dec << len;
    }
    return len;
}

quint32 Riff::readChunk(quint32& chunkType)
{
    quint32 len = 0;
    chunkType = read32bit();
    len = read32bit();
//    qDebug() << "chunkType:" << toString(chunkType)
//             << "(" << hex << chunkType << ")"
//             << "length:" << dec << len;
    return len;
}

quint32 Riff::readChunkID()
{
    quint32 chunkID = read32bit();
//    qDebug() << "chunkID:" << toString(chunkID)
//             << "(" << hex << chunkID << ")";
    return chunkID;
}

QString Riff::readSFVersion()
{
    int sfVersion = read16bit();
    int sfMinor = read16bit();
    return QString("%1.%2").arg(sfVersion).arg(sfMinor);
}

QString Riff::readDLSVersion()
{
    quint16 v[4];
    for(int i=0; i<4; ++i) v[i] = read16bit();
    return QString("%1.%2.%3.%4").arg(v[0]).arg(v[1]).arg(v[2]).arg(v[3]);
}

void Riff::processINFO(int size)
{
    QString str;
    quint32 chunkID = 0;
    quint32 length = 0;
    while (size > 0) {
        if (m_IOStream->atEnd()) return;
        length = readChunk(chunkID);
        if (length & 1) length++;
        size -= 8;
        switch (chunkID) {
        case CKID_IFIL:
            m_version = readSFVersion();
            break;
        case CKID_INAM:
            m_name = readstr(length);
            break;
        case CKID_ICOP:
            m_copyright = readstr(length);
            break;
        default:
            skip(length);
        }
        size -= length;
    }
}

void Riff::processPHDR(int size)
{
    int i, pc, bank;
    char name[21];
    int npresets = size / 38 - 1;
    for (i = 0; i < npresets; i++) {
        if (m_IOStream->atEnd()) return;
        m_IOStream->readRawData(name, 20);
        name[20] = 0;
        pc = read16bit();
        bank = read16bit();
        skip(14);
        if (bank < 128)
            emit signalInstrument(bank, pc, QString(name));
        else
            emit signalPercussion(bank, pc, QString(name));
        //qDebug() << "Instrument: " << bank << pc << name;
    }
    skip(38);
}

void Riff::processPDTA(int size)
{
    quint32 chunkID = 0;
    quint32 length;
    while (size > 0) {
        if (m_IOStream->atEnd()) return;
        length = readChunk(chunkID);
        size -= 8;
        if (m_IOStream->atEnd())
            return;
        if (chunkID == CKID_PHDR)
            processPHDR(length);
        else
            skip(length);
        size -= length;
    }
}

void Riff::processSFList(int size)
{
    quint32 chunkID = 0;
    if (m_IOStream->atEnd()) return;
    chunkID = readChunkID();
    size -= 4;
    switch (chunkID) {
    case CKID_INFO:
        processINFO(size);
        break;
    case CKID_PDTA:
        processPDTA(size);
        break;
    default:
        skip(size);
    }
}

void Riff::processINSH(quint32& bank, quint32& pc, bool& perc)
{
    read32bit();
    bank = read32bit();
    pc = read32bit();
    perc = (bank & 0x80000000) != 0;
    bank &= 0x3FFF;
}

void Riff::processINS(int size)
{
    bool perc = false;
    quint32 bank = 0, pc = 0;
    quint32 chunkID = 0;
    int length;
    while (size > 0) {
        if (m_IOStream->atEnd()) return;
        length = readChunk(chunkID);
        size -= 8;
        switch (chunkID) {
        case CKID_INSH:
            processINSH(bank, pc, perc);
            break;
        case CKID_LIST:
            processDLSList(length);
            break;
        default:
            skip(length);
        }
        size -= length;
    }
    //qDebug() << "Instrument:" << bank << pc << m_name;
    if (perc)
        emit signalPercussion(bank, pc, m_name);
    else
        emit signalInstrument(bank, pc, m_name);
    m_name.clear();
    m_copyright.clear();
}

void Riff::processLINS(int size)
{
    quint32 chunkID = 0;
    int length;
    while (size > 0) {
        if (m_IOStream->atEnd()) return;
        length = readChunk(chunkID);
        size -= 8;
        if (chunkID == CKID_LIST)
            processDLSList(length);
        else
            skip(length);
        size -= length;
    }
}

void Riff::processDLSList(int size)
{
    quint32 chunkID = 0;
    if (m_IOStream->atEnd()) return;
    chunkID = readChunkID();
    size -= 4;
    switch (chunkID) {
    case CKID_INFO:
        processINFO(size);
        break;
    case CKID_LINS:
        processLINS(size);
        break;
    case CKID_INS:
        processINS(size);
        break;
    default:
        skip(size);
    }
}

void Riff::processDLS(int size)
{
    quint32 chunkID = 0;
    int length;
    while (size > 0) {
        if (m_IOStream->atEnd()) return;
        length = readChunk(chunkID);
        size -= 8;
        switch (chunkID) {
        case CKID_VERS:
            m_version = readDLSVersion();
            //qDebug() << "Version: " << m_version;
            break;
        case CKID_LIST:
            processDLSList(length);
            break;
        default:
            skip(length);
        }
        size -= length;
    }
    //qDebug() << "DLS:" << m_name << m_version << m_copyright;
    emit signalDLS(m_name, m_version, m_copyright);
}

void Riff::processSF(int size)
{
    quint32 chunkID = 0;
    int length;
    while (size > 0) {
        if (m_IOStream->atEnd()) return;
        length = readChunk(chunkID);
        size -= 8;
        if (m_IOStream->atEnd())
            return;
        if (chunkID == CKID_LIST)
            processSFList(length);
        else
            skip(length);
        size -= length;
    }
    //qDebug() << "SoundFont:" << m_name << m_version << m_copyright;
    emit signalSoundFont(m_name, m_version, m_copyright);
}

void Riff::read()
{
    quint32 chunkID;
    quint32 length = readExpectedChunk(CKID_RIFF);
    if (length > 0) {
        chunkID = readChunkID();
        length -= 4;
        switch(chunkID) {
        case CKID_DLS:
            //qDebug() << "DLS format";
            processDLS(length);
            break;
        case CKID_SFBK:
            //qDebug() << "SoundFont format";
            processSF(length);
            break;
        default:
            qWarning() << "Unsupported format";
        }
    }
}
