/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2013, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    For this file, the following copyright notice is also applicable:
    Copyright (C) 2005-2013, rncbc aka Rui Nuno Capela. All rights reserved.
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

/*  Library of compatible instrument definitions:
    ftp://ftp.cakewalk.com/pub/InstrumentDefinitions/ */

#ifndef __instrument_h
#define __instrument_h

#include <QStringList>
#include <QMap>

// Forward declarations.
class QTextStream;

//----------------------------------------------------------------------
// class InstrumentData -- instrument definition data classes.
//

class InstrumentData
{
public:

	typedef QMap<int, QString> DataMap;

	// Constructor.
	InstrumentData()
		: m_pData(new DataRef()) {}
	// Copy constructor.
	InstrumentData(const InstrumentData& data)
		{ attach(data); }
	// Destructor.
	~InstrumentData()
		{ detach(); }

	// Assignment operator.
	InstrumentData& operator= (const InstrumentData& data)
	{
		if (m_pData != data.m_pData) {
			detach();
			attach(data);
		}
		return *this;
	}

	// Accessor operator.
	QString& operator[] (int iIndex) const
		{ return m_pData->map[iIndex]; }

	// Property accessors.
	void setName(const QString& sName)
		{ m_pData->name = sName; }
	const QString& name() const { return m_pData->name; }

	void setBasedOn(const QString& sBasedOn)
		{ m_pData->basedOn = sBasedOn; }
	const QString& basedOn() const { return m_pData->basedOn; }

	// Indirect iterator stuff.
	typedef DataMap::Iterator Iterator;
	Iterator begin() { return m_pData->map.begin(); }
	Iterator end()   { return m_pData->map.end(); }

	typedef DataMap::ConstIterator ConstIterator;
	ConstIterator constBegin() const { return m_pData->map.constBegin(); }
	ConstIterator constEnd()   const { return m_pData->map.constEnd(); }

	unsigned int count() const { return m_pData->map.count(); }

	bool contains(int iKey) const
		{ return m_pData->map.contains(iKey); }

protected:

	// Copy/clone method.
	void attach(const InstrumentData& data)
		{  m_pData = data.m_pData; m_pData->refCount++; }
	// Destroy method.
	void detach()
		{ if (--(m_pData->refCount) == 0) delete m_pData; }

private:

	// The ref-counted data.
	struct DataRef
	{
		// Default payload constructor.
		DataRef() : refCount(1) {};
		// Payload members.
		int     refCount;
		QString name;
		QString basedOn;
		DataMap map;

	} * m_pData;
};

class InstrumentDataList
	: public QMap<QString, InstrumentData> {};

class InstrumentPatches
	: public QMap<int, InstrumentData> {};

class InstrumentNotes
	: public QMap<int, InstrumentData> {};

class InstrumentKeys
	: public QMap<int, InstrumentNotes> {};

class InstrumentDrumFlags
	: public QMap<int, int> {};

class InstrumentDrums
	: public QMap<int, InstrumentDrumFlags> {};


//----------------------------------------------------------------------
// class Instrument -- instrument definition instance class.
//

class Instrument
{
public:

	// Constructor.
	Instrument()
		: m_pData(new DataRef()) {}
	// Copy constructor.
	Instrument(const Instrument& instr)
		{ attach(instr); }
	// Destructor.
	~Instrument()
		{ detach(); }

	// Assignment operator.
	Instrument& operator= (const Instrument& instr)
	{
		if (m_pData != instr.m_pData) {
			detach();
			attach(instr);
		}
		return *this;
	}

	// Instrument title property accessors.
	void setInstrumentName(const QString& sInstrumentName)
		{ m_pData->instrumentName = sInstrumentName; }
	const QString& instrumentName() const
		{ return m_pData->instrumentName; }

	// BankSelMethod accessors.
	void setBankSelMethod(int iBankSelMethod)
		{ m_pData->bankSelMethod = iBankSelMethod; }
	int bankSelMethod() const { return m_pData->bankSelMethod; }

	void setUsesNotesAsControllers(bool bUsesNotesAsControllers)
		{ m_pData->usesNotesAsControllers = bUsesNotesAsControllers; }
	bool usesNotesAsControllers() const
		{ return m_pData->usesNotesAsControllers; }

	// Patch banks accessors.
	const InstrumentPatches& patches() const
		{ return m_pData->patches; }
	const InstrumentData& patch(int iBank) const;
	void setPatch(int iBank, const InstrumentData& patch)
		{ m_pData->patches[iBank] = patch; }

	// Control names accessors.
	void setControlName(const QString& sControlName)
		{ m_pData->control.setName(sControlName); }
	const QString& controlName() const
		{ return m_pData->control.name(); }
	void setControl(const InstrumentData& control)
		{ m_pData->control = control; }
	const InstrumentData& control() const
		{ return m_pData->control; }

	// RPN names accessors.
	void setRpnName(const QString& sRpnName)
		{ m_pData->rpn.setName(sRpnName); }
	const QString& rpnName() const
		{ return m_pData->rpn.name(); }
	void setRpn(const InstrumentData& rpn)
		{ m_pData->rpn = rpn; }
	const InstrumentData& rpn() const
		{ return m_pData->rpn; }

	// NRPN names accessors.
	void setNrpnName(const QString& sNrpnName)
		{ m_pData->nrpn.setName(sNrpnName); }
	const QString& nrpnName() const
		{ return m_pData->nrpn.name(); }
	void setNrpn(const InstrumentData& nrpn)
		{ m_pData->nrpn = nrpn; }
	const InstrumentData& nrpn() const
		{ return m_pData->nrpn; }

	// Keys banks accessors.
	const InstrumentData& notes(int iBank, int iProg) const;
	void setNotes(int iBank, int iProg, const InstrumentData& notes)
		{ m_pData->keys[iBank][iProg] = notes; }
	const InstrumentKeys& keys() const
		{ return m_pData->keys; }

	// Drumflags banks accessors.
	bool isDrum(int iBank, int iProg) const;
	void setDrum(int iBank, int iProg, bool bDrum)
		{ m_pData->drums[iBank][iProg] = (int) bDrum; }
	const InstrumentDrums& drums() const
		{ return m_pData->drums; }

protected:

	// Copy/clone method.
	void attach(const Instrument& instr)
		{  m_pData = instr.m_pData; m_pData->refCount++; }
	// Destroy method.
	void detach()
		{ if (--(m_pData->refCount) == 0) delete m_pData; }

private:

	// The ref-counted data.
	struct DataRef
	{
		// Default payload constructor.
		DataRef() : refCount(1),
			bankSelMethod(0), usesNotesAsControllers(false) {};
		// Payload members.
		int                       refCount;
		int                       bankSelMethod;
		bool                      usesNotesAsControllers;
		QString                   instrumentName;
		InstrumentPatches patches;
		InstrumentData    control;
		InstrumentData    rpn;
		InstrumentData    nrpn;
		InstrumentKeys    keys;
		InstrumentDrums   drums;

	} * m_pData;
};


//----------------------------------------------------------------------
// class InstrumentList -- A Cakewalk .ins file container class.
//

class InstrumentList : public QMap<QString, Instrument>
{
public:

	// Open file methods.
	bool load(const QString& sFilename);
	bool save(const QString& sFilename);

	// The official loaded file list.
	const QStringList& files() const;

	// Manage a file list (out of sync)
	void appendFile(const QString& sFilename)
	    { m_files.append(sFilename); }
	void removeFile(const QString& sFilename)
	{
		int iFile = m_files.indexOf(sFilename);
		if (iFile >= 0)
			m_files.removeAt(iFile);
	}

	// Patch Names definition accessors.
	const InstrumentDataList& patches() const
		{ return m_patches; }
	const InstrumentData& patch(const QString& sName)
		{ return m_patches[sName]; }

	// Note Names definition accessors.
	const InstrumentDataList& notes() const
		{ return m_notes; }
	InstrumentData& note(const QString& sName)
		{ return m_notes[sName]; }

	// Controller Names definition accessors.
	const InstrumentDataList& controllers() const
		{ return m_controllers; }
	InstrumentData& controller(const QString& sName)
		{ return m_controllers[sName]; }

	// RPN Names definition accessors.
	const InstrumentDataList& rpns() const
		{ return m_rpns; }
	InstrumentData& rpn(const QString& sName)
		{ return m_rpns[sName]; }

	// NRPN Names definition accessors.
	const InstrumentDataList& nrpns() const
		{ return m_nrpns; }
	InstrumentData& nrpn(const QString& sName)
		{ return m_nrpns[sName]; }

	// Clear all contents.
	void clearAll();

	// Special instrument list merge method.
	void merge(const InstrumentList& instruments);

protected:

	// Internal instrument data list save method helpers.
	void saveDataList(QTextStream& ts, const InstrumentDataList& list);
	void saveData(QTextStream& ts, const InstrumentData& data);

	// Special instrument data list merge method.
	void mergeDataList(InstrumentDataList& dst,
		const InstrumentDataList& src);

private:

	// To hold the names definition lists.
	InstrumentDataList m_patches;
	InstrumentDataList m_notes;
	InstrumentDataList m_controllers;
	InstrumentDataList m_rpns;
	InstrumentDataList m_nrpns;

	// To old the official file list.
	QStringList m_files;
};


#endif  // __instrument_h
