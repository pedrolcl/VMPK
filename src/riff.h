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

#ifndef RIFF_H
#define RIFF_H

#include <QObject>
#include <QDataStream>

#define CKID_RIFF 0x46464952
#define CKID_LIST 0x5453494c

#define CKID_SFBK 0x6b626673
#define CKID_INFO 0x4f464e49
#define CKID_PDTA 0x61746470
#define CKID_IFIL 0x6c696669
#define CKID_INAM 0x4d414e49
#define CKID_PHDR 0x72646870

#define CKID_DLS  0x20534C44
#define CKID_COLH 0x686c6f63
#define CKID_VERS 0x73726576
#define CKID_LINS 0x736e696c
#define CKID_ICOP 0x504f4349
#define CKID_INS  0x20736e69
#define CKID_INSH 0x68736e69

class Riff : public QObject
{
    Q_OBJECT

public:
    Riff(QObject* parent = 0);
    virtual ~Riff();
    void readFromFile(QString fileName);
    void readFromStream(QDataStream* ds);

signals:
    void signalSoundFont(QString name, QString version, QString copyright);
    void signalDLS(QString name, QString version, QString copyright);
    void signalInstrument(int bank, int pc, QString name);
    void signalPercussion(int bank, int pc, QString name);

private:
    /* SoundFont */
    void processSF(int size);
    void processSFList(int size);
    void processPDTA(int size);
    void processPHDR(int size);
    QString readSFVersion();
    /* DLS */
    void processDLS(int size);
    void processDLSList(int size);
    void processLINS(int size);
    void processINS(int size);
    void processINSH(quint32& bank, quint32& pc, bool& perc);
    QString readDLSVersion();
    /* common */
    void read();
    void processINFO(int size);
    quint32 readExpectedChunk(quint32 cktype);
    quint32 readChunk(quint32& cktype);
    quint32 readChunkID();
    quint16 read16bit();
    quint32 read32bit();
    QString readstr(int size);
    void skip(int size);

    QDataStream *m_IOStream;
    QString m_fileName;
    QString m_name;
    QString m_copyright;
    QString m_version;
};

#endif // RIFF_H
