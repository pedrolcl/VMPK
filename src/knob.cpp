/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2013, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    For this file, the following copyright notice is also applicable:
    Copyright (C) 2005-2013, rncbc aka Rui Nuno Capela. All rights reserved.
    See http://qsynth.sourceforge.net

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
#define _USE_MATH_DEFINES // MSVC

#include "knob.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <cmath>

//-------------------------------------------------------------------------
// Knob - Instance knob widget class.
//
// Constructor.
Knob::Knob ( QWidget *pParent )
	: QDial(pParent), m_iDefaultValue(-1), m_dialMode(AngularMode),
	m_bMousePressed(false), m_lastDragValue(0.0f)
{}

void Knob::setDefaultValue ( int iDefaultValue )
{
    m_iDefaultValue = iDefaultValue;
}

void Knob::setDialMode ( Knob::DialMode dialMode )
{
    m_dialMode = dialMode;
}

// Mouse angle determination.
float Knob::mouseAngle ( const QPoint& pos )
{
    float dx = pos.x() - (width() / 2);
    float dy = (height() / 2) - pos.y();
    return 180.0f * atan2f(dx, dy) / float(M_PI);
}

// Alternate mouse behavior event handlers.
void Knob::mousePressEvent ( QMouseEvent *pMouseEvent )
{
    if (m_dialMode == QDialMode) {
        QDial::mousePressEvent(pMouseEvent);
    } else if (pMouseEvent->button() == Qt::LeftButton) {
        m_bMousePressed = true;
        m_posMouse = pMouseEvent->pos();
        m_lastDragValue = float(sliderPosition());
        emit sliderPressed();
    } else if (pMouseEvent->button() == Qt::MidButton) {
        // Reset to default value...
        if (m_iDefaultValue < minimum() || m_iDefaultValue > maximum())
            m_iDefaultValue = (maximum() + minimum()) / 2;
        updatePosition(m_iDefaultValue);
    }
}

void Knob::mouseMoveEvent ( QMouseEvent *pMouseEvent )
{
    if (m_dialMode == QDialMode) {
        QDial::mouseMoveEvent(pMouseEvent);
        return;
    }

    if (!m_bMousePressed)
        return;

    const QPoint& posMouse = pMouseEvent->pos();
    int xdelta = posMouse.x() - m_posMouse.x();
    int ydelta = posMouse.y() - m_posMouse.y();
    float angleDelta =  mouseAngle(posMouse) - mouseAngle(m_posMouse);

    int iNewValue = sliderPosition();

    switch (m_dialMode)	{
    case LinearMode:
        iNewValue = int(m_lastDragValue + xdelta - ydelta);
        if (iNewValue > maximum())
            iNewValue = maximum();
        else
            if (iNewValue < minimum())
                iNewValue = minimum();
        break;
    case AngularMode:
    default:
        // Forget about the drag origin to be robust on full rotations
        if (angleDelta > +180.0f) angleDelta = angleDelta - 360.0f;
        if (angleDelta < -180.0f) angleDelta = angleDelta + 360.0f;
        m_lastDragValue += float(maximum() - minimum()) * angleDelta / 270.0f;
        if (m_lastDragValue > float(maximum()))
            m_lastDragValue = float(maximum());
        if (m_lastDragValue < float(minimum()))
            m_lastDragValue = float(minimum());
        m_posMouse = posMouse;
        iNewValue = int(m_lastDragValue + 0.5f);
        break;
    }
    updatePosition(iNewValue);
}

void Knob::mouseReleaseEvent ( QMouseEvent *pMouseEvent )
{
    if (m_dialMode == QDialMode) {
        QDial::mouseReleaseEvent(pMouseEvent);
    } else if (m_bMousePressed) {
        m_bMousePressed = false;
    }
}

void Knob::wheelEvent ( QWheelEvent *pWheelEvent )
{
    if (m_dialMode == QDialMode) {
        QDial::wheelEvent(pWheelEvent);
    } else {
        int iValue = sliderPosition();
        if (pWheelEvent->delta() > 0)
            iValue -= pageStep();
        else
            iValue += pageStep();
        if (iValue > maximum())
            iValue = maximum();
        else
            if (iValue < minimum())
                iValue = minimum();
        updatePosition(iValue);
    }
}

void Knob::updatePosition(int val)
{
    setSliderPosition(val);
    setToolTip(QString::number(val));
    update();
    emit sliderMoved(val);
}

// end of knob.cpp
