/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2024, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    For this file, the following copyright notice is also applicable:
    Copyright (C) 2005-2024, rncbc aka Rui Nuno Capela. All rights reserved.
    See http://qtractor.sourceforge.net

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

#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QRegularExpression>
#include "instrument.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
#define endl Qt::endl
#endif

//----------------------------------------------------------------------
// class Instrument -- instrument definition instance class.
//

// Retrieve patch/program list for given bank address.
const InstrumentData& Instrument::patch ( int iBank ) const
{
    if (m_pData->patches.contains(iBank))
        return m_pData->patches[iBank];

    return m_pData->patches[-1];
}


// Retrieve key/notes list for given (bank, prog) pair.
const InstrumentData& Instrument::notes ( int iBank, int iProg ) const
{
    if (m_pData->keys.contains(iBank)) {
        if (m_pData->keys[iBank].contains(iProg)) {
            return m_pData->keys[iBank][iProg];
        } else {
            return m_pData->keys[iBank][-1];
        }
    }
    else if (iBank >= 0)
        return notes(-1, iProg);

    return m_pData->keys[-1][-1];
}


// Check if given (bank, prog) pair is a drum patch.
bool Instrument::isDrum ( int iBank, int iProg ) const
{
    if (m_pData->drums.contains(iBank)) {
        if (m_pData->drums[iBank].contains(iProg)) {
            return (bool) m_pData->drums[iBank][iProg];
        } else {
            return (bool) m_pData->drums[iBank][-1];
        }
    }
    else if (iBank >= 0)
        return isDrum(-1, iProg);

    return false;

    return isDrum(-1, iProg);
}


//----------------------------------------------------------------------
// class InstrumentList -- A Cakewalk .ins file container class.
//

// Clear all contents.
void InstrumentList::clearAll (void)
{
    clear();

    m_patches.clear();
    m_notes.clear();
    m_controllers.clear();
    m_rpns.clear();
    m_nrpns.clear();

    m_files.clear();
}


// Special list merge method.
void InstrumentList::merge ( const InstrumentList& instruments )
{
    // Maybe its better not merging to itself.
    if (this == &instruments)
        return;

    // Names data lists merge...
    mergeDataList(m_patches, instruments.patches());
    mergeDataList(m_notes, instruments.notes());
    mergeDataList(m_controllers, instruments.controllers());
    mergeDataList(m_rpns, instruments.rpns());
    mergeDataList(m_nrpns, instruments.nrpns());

    // Instrument merge...
    InstrumentList::ConstIterator it;
    for (it = instruments.begin(); it != instruments.end(); ++it) {
        Instrument& instr = (*this)[it.key()];
        instr = it.value();
    }
}


// Special instrument data list merge method.
void InstrumentList::mergeDataList (
        InstrumentDataList& dst, const InstrumentDataList& src )
{
    InstrumentDataList::ConstIterator it;
    for (it = src.begin(); it != src.end(); ++it)
        dst[it.key()] = it.value();
}


// The official loaded file list.
const QStringList& InstrumentList::files (void) const
{
    return m_files;
}


// File load method.
bool InstrumentList::load ( const QString& sFilename )
{
    // Open and read from real file.
    QFile file(sFilename);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    enum FileSection {
        None         = 0,
        PatchNames   = 1,
        NoteNames    = 2,
        ControlNames = 3,
        RpnNames     = 4,
        NrpnNames    = 5,
        InstrDefs    = 6
    } sect = None;

    Instrument     *pInstrument = nullptr;
    InstrumentData *pData = nullptr;

    QRegularExpression rxTitle   ("^\\[([^\\]]+)\\]$");
    QRegularExpression rxData    ("^([0-9]+)=(.*)$");
    QRegularExpression rxBasedOn ("^BasedOn=(.+)$");
    QRegularExpression rxBankSel ("^BankSelMethod=(0|1|2|3)$");
    QRegularExpression rxUseNotes("^UsesNotesAsControllers=(0|1)$");
    QRegularExpression rxControl ("^Control=(.+)$");
    QRegularExpression rxRpn     ("^RPN=(.+)$");
    QRegularExpression rxNrpn    ("^NRPN=(.+)$");
    QRegularExpression rxPatch   ("^Patch\\[([0-9]+|\\*)\\]=(.+)$");
    QRegularExpression rxKey     ("^Key\\[([0-9]+|\\*),([0-9]+|\\*)\\]=(.+)$");
    QRegularExpression rxDrum    ("^Drum\\[([0-9]+|\\*),([0-9]+|\\*)\\]=(0|1)$");

    QRegularExpressionMatch match;

    const QString s0_127    = "0..127";
    const QString s1_128    = "1..128";
    const QString s0_16383  = "0..16383";
    const QString sAsterisk = "*";

    // Read the file.
    unsigned int iLine = 0;
    QTextStream ts(&file);

    while (!ts.atEnd()) {

        // Read the line.
        iLine++;
        QString sLine = ts.readLine().simplified();
        // If not empty, nor a comment, call the server...
        if (sLine.isEmpty() || sLine[0] == ';')
            continue;

        // Check for section intro line...
        if (sLine[0] == '.') {
            if (sLine == ".Patch Names") {
                sect = PatchNames;
                //	m_patches.clear();
                m_patches[s0_127].setName(s0_127);
                m_patches[s1_128].setName(s1_128);
            }
            else if (sLine == ".Note Names") {
                sect = NoteNames;
                //	m_notes.clear();
                m_notes[s0_127].setName(s0_127);
            }
            else if (sLine == ".Controller Names") {
                sect = ControlNames;
                //	m_controllers.clear();
                m_controllers[s0_127].setName(s0_127);
            }
            else if (sLine == ".RPN Names") {
                sect = RpnNames;
                //	m_rpns.clear();
                m_rpns[s0_16383].setName(s0_16383);
            }
            else if (sLine == ".NRPN Names") {
                sect = NrpnNames;
                //	m_nrpns.clear();
                m_nrpns[s0_16383].setName(s0_16383);
            }
            else if (sLine == ".Instrument Definitions") {
                sect = InstrDefs;
                //  clear();
            }
            else {
                // Unknown section found...
                qWarning("%s(%d): %s: Unknown section.",
                         sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
            }
            // Go on...
            continue;
        }

        // Now it depends on the section...
        switch (sect) {
        case PatchNames: {
            match = rxTitle.match(sLine);
            if (match.hasMatch()) {
                // New patch name...
                const QString& sTitle = match.captured(1);
                pData = &(m_patches[sTitle]);
                pData->setName(sTitle);
                break;
            }
            if (pData == nullptr) {
                qWarning("%s(%d): %s: Untitled .Patch Names entry.",
                    sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
                break;
            }
            match = rxBasedOn.match(sLine);
            if (match.hasMatch()) {
                pData->setBasedOn(match.captured(1));
                break;
            }
            match = rxData.match(sLine);
            if (match.hasMatch()) {
                (*pData)[match.captured(1).toInt()] = match.captured(2);
            } else {
                qWarning("%s(%d): %s: Unknown .Patch Names entry.",
                    sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
            }
            break;
        }
        case NoteNames: {
            match = rxTitle.match(sLine);
            if (match.hasMatch()) {
                // New note name...
                const QString& sTitle = match.captured(1);
                pData = &(m_notes[sTitle]);
                pData->setName(sTitle);
                break;
            }
            if (pData == nullptr) {
                qWarning("%s(%d): %s: Untitled .Note Names entry.",
                    sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
                break;
            }
            match = rxBasedOn.match(sLine);
            if (match.hasMatch()) {
                pData->setBasedOn(match.captured(1));
                break;
            }
            match = rxData.match(sLine);
            if (match.hasMatch()) {
                (*pData)[match.captured(1).toInt()] = match.captured(2);
            } else {
                qWarning("%s(%d): %s: Unknown .Note Names entry.",
                    sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
            }
            break;
        }
        case ControlNames: {
            match = rxTitle.match(sLine);
            if (match.hasMatch()) {
                // New controller name...
                const QString& sTitle = match.captured(1);
                pData = &(m_controllers[sTitle]);
                pData->setName(sTitle);
                break;
            }
            if (pData == nullptr) {
                qWarning("%s(%d): %s: Untitled .Controller Names entry.",
                    sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
                break;
            }
            match = rxBasedOn.match(sLine);
            if (match.hasMatch()) {
                pData->setBasedOn(match.captured(1));
                break;
            }
            match = rxData.match(sLine);
            if (match.hasMatch()) {
                (*pData)[match.captured(1).toInt()] = match.captured(2);
            } else {
                qWarning("%s(%d): %s: Unknown .Controller Names entry.",
                    sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
            }
            break;
        }
        case RpnNames: {
            match = rxTitle.match(sLine);
            if (match.hasMatch()) {
                // New RPN name...
                const QString& sTitle = match.captured(1);
                pData = &(m_rpns[sTitle]);
                pData->setName(sTitle);
                break;
            }
            if (pData == nullptr) {
                qWarning("%s(%d): %s: Untitled .RPN Names entry.",
                    sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
                break;
            }
            match = rxBasedOn.match(sLine);
            if (match.hasMatch()) {
                pData->setBasedOn(match.captured(1));
                break;
            }
            match = rxData.match(sLine);
            if (match.hasMatch()) {
                (*pData)[match.captured(1).toInt()] = match.captured(2);
            } else {
                qWarning("%s(%d): %s: Unknown .RPN Names entry.",
                    sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
            }
            break;
        }
        case NrpnNames: {
            match = rxTitle.match(sLine);
            if (match.hasMatch()) {
                // New NRPN name...
                const QString& sTitle = match.captured(1);
                pData = &(m_nrpns[sTitle]);
                pData->setName(sTitle);
                break;
            }
            if (pData == nullptr) {
                qWarning("%s(%d): %s: Untitled .NRPN Names entry.",
                    sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
                break;
            }
            match = rxBasedOn.match(sLine);
            if (match.hasMatch()) {
                pData->setBasedOn(match.captured(1));
                break;
            }
            match = rxData.match(sLine);
            if (match.hasMatch()) {
                (*pData)[match.captured(1).toInt()] = match.captured(2);
            } else {
                qWarning("%s(%d): %s: Unknown .NRPN Names entry.",
                    sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
            }
            break;
        }
        case InstrDefs: {
            match = rxTitle.match(sLine);
            if (match.hasMatch()) {
                // New instrument definition...
                const QString& sTitle = match.captured(1);
                pInstrument = &((*this)[sTitle]);
                pInstrument->setInstrumentName(sTitle);
                break;
            }
            if (pInstrument == nullptr) {
                // New instrument definition (use filename as default)
                const QString& sTitle = QFileInfo(sFilename).completeBaseName();
                pInstrument = &((*this)[sTitle]);
                pInstrument->setInstrumentName(sTitle);
            }
            match = rxBankSel.match(sLine);
            if (match.hasMatch()) {
                pInstrument->setBankSelMethod(
                    match.captured(1).toInt());
                break;
            }
            match = rxUseNotes.match(sLine);
            if (match.hasMatch()) {
                pInstrument->setUsesNotesAsControllers(
                    bool(match.captured(1).toInt()));
                break;
            }
            match = rxPatch.match(sLine);
            if (match.hasMatch()) {
                const QString& cap1 = match.captured(1);
                const int iBank = (cap1 == sAsterisk ? -1 : cap1.toInt());
                pInstrument->setPatch(iBank, m_patches[match.captured(2)]);
                break;
            }
            match = rxControl.match(sLine);
            if (match.hasMatch()) {
                pInstrument->setControl(m_controllers[match.captured(1)]);
                break;
            }
            match = rxRpn.match(sLine);
            if (match.hasMatch()) {
                pInstrument->setRpn(m_rpns[match.captured(1)]);
                break;
            }
            match = rxNrpn.match(sLine);
            if (match.hasMatch()) {
                pInstrument->setNrpn(m_nrpns[match.captured(1)]);
                break;
            }
            match = rxKey.match(sLine);
            if (match.hasMatch()) {
                const QString& cap1 = match.captured(1);
                const QString& cap2 = match.captured(2);
                const int iBank = (cap1 == sAsterisk ? -1 : cap1.toInt());
                const int iProg = (cap2 == sAsterisk ? -1 : cap2.toInt());
                pInstrument->setNotes(iBank, iProg,	m_notes[match.captured(3)]);
                break;
            }
            match = rxDrum.match(sLine);
            if (match.hasMatch()) {
                const QString& cap1 = match.captured(1);
                const QString& cap2 = match.captured(2);
                const int iBank = (cap1 == sAsterisk ? -1 : cap1.toInt());
                const int iProg = (cap2 == sAsterisk ? -1 : cap2.toInt());
                pInstrument->setDrum(iBank, iProg,
                    bool(match.captured(3).toInt()));
            } else {
                qWarning("%s(%d): %s: Unknown .Instrument Definitions entry.",
                    sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
            }
            break;
        }
        default:
            break;
        }
    }

    // Ok. We've read it all.
    file.close();

    // We're in business...
    appendFile(sFilename);

    return true;
}


// File save method.
bool InstrumentList::save ( const QString& sFilename )
{
    // Open and write into real file.
    QFile file(sFilename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    // A visula separator line.
    const QString sepl = "; -----------------------------"
                         "------------------------------------------------";

    // Write the file.
    QTextStream ts(&file);

    ts << sepl << endl;
    ts << "; " << QObject::tr("Cakewalk Instrument Definition File") << endl;
    /*
    ts << ";"  << endl;
    ts << "; " << _TITLE " - " << QObject::tr(_SUBTITLE) << endl;
    ts << "; " << QObject::tr("Version")
       << ": " _VERSION << endl;
    ts << "; " << QObject::tr("Build")
       << ": " __DATE__ " " __TIME__ << endl;
*/
    ts << ";"  << endl;
    ts << "; " << QObject::tr("File")
       << ": " << QFileInfo(sFilename).fileName() << endl;
    ts << "; " << QObject::tr("Date")
       << ": " << QDate::currentDate().toString("MMM dd yyyy")
       << " "  << QTime::currentTime().toString("hh:mm:ss") << endl;
    ts << ";"  << endl;

    // - Patch Names...
    ts << sepl << endl << endl;
    ts << ".Patch Names" << endl;
    saveDataList(ts, m_patches);

    // - Note Names...
    ts << sepl << endl << endl;
    ts << ".Note Names" << endl;
    saveDataList(ts, m_notes);

    // - Controller Names...
    ts << sepl << endl << endl;
    ts << ".Controller Names" << endl;
    saveDataList(ts, m_controllers);

    // - RPN Names...
    ts << sepl << endl << endl;
    ts << ".RPN Names" << endl;
    saveDataList(ts, m_rpns);

    // - NRPN Names...
    ts << sepl << endl << endl;
    ts << ".NRPN Names" << endl;
    saveDataList(ts, m_nrpns);

    // - Instrument Definitions...
    ts << sepl << endl << endl;
    ts << ".Instrument Definitions" << endl;
    ts << endl;
    InstrumentList::Iterator iter;
    for (iter = begin(); iter != end(); ++iter) {
        Instrument& instr = *iter;
        ts << "[" << instr.instrumentName() << "]" << endl;
        if (instr.bankSelMethod() > 0)
            ts << "BankSelMethod=" << instr.bankSelMethod() << endl;
        if (!instr.control().name().isEmpty())
            ts << "Control=" << instr.control().name() << endl;
        if (!instr.rpn().name().isEmpty())
            ts << "RPN=" << instr.rpn().name() << endl;
        if (!instr.nrpn().name().isEmpty())
            ts << "NRPN=" << instr.nrpn().name() << endl;
        // - Patches...
        InstrumentPatches::ConstIterator pit;
        for (pit = instr.patches().begin();
             pit != instr.patches().end(); ++pit) {
            int iBank = pit.key();
            const QString sBank = (iBank < 0
                                   ? QString("*") : QString::number(iBank));
            ts << "Patch[" << sBank << "]=" << pit.value().name() << endl;
        }
        // - Keys...
        InstrumentKeys::ConstIterator kit;
        for (kit = instr.keys().begin(); kit != instr.keys().end(); ++kit) {
            int iBank = kit.key();
            const QString sBank = (iBank < 0
                                   ? QString("*") : QString::number(iBank));
            const InstrumentNotes& notes = kit.value();
            InstrumentNotes::ConstIterator nit;
            for (nit = notes.begin(); nit != notes.end(); ++nit) {
                int iProg = nit.key();
                const QString sProg = (iProg < 0
                                       ? QString("*") : QString::number(iProg));
                ts << "Key[" << sBank << "," << sProg << "]="
                   << nit.value().name() << endl;
            }
        }
        // - Drums...
        InstrumentDrums::ConstIterator dit;
        for (dit = instr.drums().begin(); dit != instr.drums().end(); ++dit) {
            int iBank = dit.key();
            const QString sBank = (iBank < 0
                                   ? QString("*") : QString::number(iBank));
            const InstrumentDrumFlags& flags = dit.value();
            InstrumentDrumFlags::ConstIterator fit;
            for (fit = flags.begin(); fit != flags.end(); ++fit) {
                int iProg = fit.key();
                const QString sProg = (iProg < 0
                                       ? QString("*") : QString::number(iProg));
                ts << "Drum[" << sBank << "," << sProg << "]="
                   << fit.value() << endl;
            }
        }
        ts << endl;
    }

    // Done.
    file.close();

    return true;
}


void InstrumentList::saveDataList ( QTextStream& ts,
                                    const InstrumentDataList& list )
{
    ts << endl;
    InstrumentDataList::ConstIterator it;
    for (it = list.begin(); it != list.end(); ++it) {
        ts << "[" << it.value().name() << "]" << endl;
        saveData(ts, it.value());
    }
}


void InstrumentList::saveData ( QTextStream& ts,
                                const InstrumentData& data )
{
    if (!data.basedOn().isEmpty())
        ts << "BasedOn=" << data.basedOn() << endl;
    InstrumentData::ConstIterator it;
    for (it = data.constBegin(); it != data.constEnd(); ++it)
        ts << it.key() << "=" << it.value() << endl;
    ts << endl;
}
