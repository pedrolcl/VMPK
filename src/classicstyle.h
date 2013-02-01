/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2013, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    Based on an original design by Thorsten Wilms.
    Implemented as a widget for the Rosegarden MIDI and audio sequencer
    and notation editor by Chris Cannam.
    Extracted into a standalone Qt3 widget by Pedro Lopez-Cabanillas
    and adapted for use in QSynth.
    Ported to Qt4 by Chris Cannam.
    Adapted as a QStyle by Pedro Lopez-Cabanillas.

    For this file, the following copyright notices are also applicable:
    This file copyright 2003-2006 Chris Cannam,
    copyright 2005,2008 Pedro Lopez-Cabanillas,
    copyright 2006 Queen Mary, University of London.

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

#ifndef CLASSICTYLE_H_
#define CLASSICTYLE_H_

#include <QCommonStyle>

class ClassicStyle : public QCommonStyle
{
public:
	ClassicStyle() {};
	virtual ~ClassicStyle() {};
    virtual void drawPrimitive( PrimitiveElement pe, const QStyleOption *opt,
                                QPainter *p, const QWidget *w ) const;
	virtual void drawComplexControl( ComplexControl cc,
	                                 const QStyleOptionComplex *opt,
	                                 QPainter *p,
                            		 const QWidget *widget = 0) const;
	virtual int pixelMetric( PixelMetric metric, const QStyleOption* option=0,
	                         const QWidget *widget = 0 ) const;
};

#endif /*CLASSICTYLE_H_*/
