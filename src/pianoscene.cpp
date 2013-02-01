/*
    Virtual Piano Widget for Qt4 
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

#include "pianoscene.h"
#if defined(RAWKBD_SUPPORT)
#include "rawkeybdapp.h"
#endif
#include <QApplication>
#include <QPalette>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QDebug>

#define KEYWIDTH  18
#define KEYHEIGHT 72

PianoScene::PianoScene ( const int baseOctave, 
                         const int numOctaves, 
                         const QColor& keyPressedColor,
                         QObject * parent )
    : QGraphicsScene( QRectF(0, 0, KEYWIDTH * numOctaves * 7, KEYHEIGHT), parent ),
    m_baseOctave( baseOctave ),
    m_numOctaves( numOctaves ),
    m_minNote( 0 ),
    m_maxNote( 127 ),
    m_transpose( 0 ),
    m_showLabels( false ),
    m_useFlats( false ),
    m_rawkbd( false ),
    m_keyboardEnabled( true ),
    m_mouseEnabled( true ),
    m_touchEnabled( true ),
    m_keyPressedColor( keyPressedColor ),
    m_mousePressed( false ),
    m_velocity( 100 ),
    m_channel( 0 ),
    m_showColorScale( false ),
    m_handler( 0 ),
    m_palette( 0 ),
    m_scalePalette( 0 )
{
    QBrush hilightBrush(m_keyPressedColor.isValid() ? m_keyPressedColor : QApplication::palette().highlight());
    QFont lblFont(QApplication::font());
    int i, numkeys = m_numOctaves * 12;
    lblFont.setPointSize(KEYLABELFONTSIZE);
    for(i = 0; i < numkeys; ++i)
    {
        float x = 0;
        PianoKey* key = NULL;
        KeyLabel* lbl = NULL;
        int octave = i / 12 * 7;
        int j = i % 12;
        if (j >= 5) j++;
        if ((j % 2) == 0) {
            x = (octave + j / 2) * KEYWIDTH;
            key = new PianoKey( QRectF(x, 0, KEYWIDTH, KEYHEIGHT), false, i );
            lbl = new KeyLabel(key);
            lbl->setDefaultTextColor(Qt::black);
            lbl->setPos(x, KEYHEIGHT);
        } else {
            x = (octave + j / 2) * KEYWIDTH + KEYWIDTH * 6/10 + 1;
            key = new PianoKey( QRectF( x, 0, KEYWIDTH * 8/10 - 1, KEYHEIGHT * 6/10 ), true, i );
            key->setZValue( 1 );
            lbl = new KeyLabel(key);
            lbl->setDefaultTextColor(Qt::white);
            lbl->setPos(x - 3, KEYHEIGHT * 6/10 - 3);
        }
        key->setAcceptTouchEvents(true);
        if (m_keyPressedColor.isValid())
            key->setPressedBrush(hilightBrush);
        m_keys.insert(i, key);
        addItem( key );
        lbl->setFont(lblFont);
        m_labels.insert(i, lbl);
    }
    hideOrShowKeys();
    retranslate();
}

QSize PianoScene::sizeHint() const
{
    return QSize(KEYWIDTH * m_numOctaves * 7, KEYHEIGHT);
}

void PianoScene::showKeyOn( PianoKey* key, QColor color, int vel )
{
    if (vel >= 0 && color.isValid() ) {
        QBrush hilightBrush(color.lighter(200 - vel));
        key->setPressedBrush(hilightBrush);
    }
    key->setPressed(true);
}

void PianoScene::showKeyOn( PianoKey* key, int vel )
{
    if (vel >= 0) {
        if (m_palette == 0 && m_keyPressedColor.isValid()) {
            QBrush hilightBrush(m_keyPressedColor.lighter(200 - vel));
            key->setPressedBrush(hilightBrush);
        } else {
            setColorFromPolicy(key, vel);
        }
    }
    key->setPressed(true);
}

void PianoScene::showKeyOff( PianoKey* key, int )
{
    key->setPressed(false);
}

void PianoScene::showNoteOn( const int note, QColor color, int vel )
{
    int n = note - m_baseOctave*12 - m_transpose;
    if ((note >= m_minNote) && (note <= m_maxNote) &&
        (n >= 0) && (n < m_keys.size()) && color.isValid())
        showKeyOn(m_keys[n], color, vel);
}

void PianoScene::showNoteOn( const int note, int vel )
{
    int n = note - m_baseOctave*12 - m_transpose;
    if ((note >= m_minNote) && (note <= m_maxNote) &&
        (n >= 0) && (n < m_keys.size()))
        showKeyOn(m_keys[n], vel);
}

void PianoScene::showNoteOff( const int note, int vel )
{
    int n = note - m_baseOctave*12 - m_transpose;
    if ((note >= m_minNote) && (note <= m_maxNote) &&
        (n >= 0) && (n < m_keys.size()))
        showKeyOff(m_keys[n], vel);
}

void PianoScene::triggerNoteOn( const int note, const int vel )
{
    int n = m_baseOctave*12 + note + m_transpose;
    if ((n >= m_minNote) && (n <= m_maxNote)) {
        if (m_handler != NULL) {
            m_handler->noteOn(n, vel);
        } else {
            emit noteOn(n, vel);
        }
    }
}

void PianoScene::triggerNoteOff( const int note, const int vel )
{
    int n = m_baseOctave*12 + note + m_transpose;
    if ((n >= m_minNote) && (n <= m_maxNote)) {
        if (m_handler != NULL) {
            m_handler->noteOff(n, vel);
        } else {
            emit noteOff(n, vel);
        }
    }
}

void PianoScene::setColorFromPolicy(PianoKey* key, int vel)
{
    QColor c;
    switch (m_palette->paletteId()) {
    case PAL_SINGLE:
        c = m_palette->getColor(0);
        break;
    case PAL_DOUBLE:
        c = m_palette->getColor(key->getType());
        break;
    case PAL_CHANNELS:
        c = m_palette->getColor(m_channel);
        break;
    case PAL_SCALE:
        c = m_palette->getColor(key->getDegree());
    }
    if (c.isValid()) {
        QBrush h(c.lighter(200 - vel));
        key->setPressedBrush(h);
    }
}

void PianoScene::keyOn( PianoKey* key )
{
    triggerNoteOn(key->getNote(), m_velocity);
    showKeyOn(key, m_velocity);
}

void PianoScene::keyOff( PianoKey* key )
{
    triggerNoteOff(key->getNote(), 0);
    showKeyOff(key, 0);
}

void PianoScene::keyOn( PianoKey* key, qreal pressure )
{
    int vel = m_velocity * pressure;
    triggerNoteOn(key->getNote(), vel);
    showKeyOn(key, vel);
}

void PianoScene::keyOff( PianoKey* key, qreal pressure )
{
    int vel = m_velocity * pressure;
    triggerNoteOff(key->getNote(), vel);
    showKeyOff(key, vel);
}

void PianoScene::keyOn(const int note)
{
    if (note >=0 && note < m_keys.size())
        keyOn(m_keys[note]);
    else
        triggerNoteOn(note, m_velocity);
}

void PianoScene::keyOff(const int note)
{
    if (note >=0 && note < m_keys.size())
        keyOff(m_keys[note]);
    else
        triggerNoteOff(note, m_velocity);
}

PianoKey* PianoScene::getKeyForPos( const QPointF& p ) const
{
    /*
    QGraphicsItem *itm = itemAt(p);
    while (itm != NULL && itm->parentItem() != NULL)
        itm = itm->parentItem();
    if (itm != NULL) {
        PianoKey* key = dynamic_cast<PianoKey*>(itm);
        return key;
    }
    return NULL;
    */

    PianoKey* key = 0;
    QList<QGraphicsItem *> ptitems = this->items(p, Qt::IntersectsItemShape, Qt::DescendingOrder);
    foreach(QGraphicsItem *itm, ptitems) {
        key = dynamic_cast<PianoKey*>(itm);
        if (key != 0)
            break;
    }
    return key;
}

void PianoScene::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if (m_mouseEnabled) {
        if (m_mousePressed) {
            PianoKey* key = getKeyForPos(mouseEvent->scenePos());
            PianoKey* lastkey = getKeyForPos(mouseEvent->lastScenePos());
            if ((lastkey != NULL) && (lastkey != key) && lastkey->isPressed()) {
                keyOff(lastkey);
            }
            if ((key != NULL) && !key->isPressed()) {
                keyOn(key);
            }
            mouseEvent->accept();
            return;
        }
    }
    //mouseEvent->ignore();
}

void PianoScene::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if (m_mouseEnabled) {
        PianoKey* key = getKeyForPos(mouseEvent->scenePos());
        if (key != NULL && !key->isPressed()) {
            keyOn(key);
            m_mousePressed = true;
            mouseEvent->accept();
            return;
        }
    }
    //mouseEvent->ignore();
}

void PianoScene::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if (m_mouseEnabled) {
        m_mousePressed = false;
        PianoKey* key = getKeyForPos(mouseEvent->scenePos());
        if (key != NULL && key->isPressed()) {
            keyOff(key);
            mouseEvent->accept();
            return;
        }
    }
    //mouseEvent->ignore();
}

int PianoScene::getNoteFromKey( const int key ) const
{
    if (m_keybdMap != NULL) {
        KeyboardMap::ConstIterator it = m_keybdMap->constFind(key);
        if ((it != m_keybdMap->constEnd()) && (it.key() == key)) {
            int note = it.value();
            return note;
        }
    }
    return -1;
}

PianoKey* PianoScene::getPianoKey( const int key ) const
{
    int note = getNoteFromKey(key);
    if ((note >= 0) && (note < m_keys.size()))
        return m_keys[note];
    return NULL;
}

void PianoScene::keyPressEvent ( QKeyEvent * keyEvent )
{
    if ( m_keyboardEnabled) {
        if ( !m_rawkbd && !keyEvent->isAutoRepeat() ) { // ignore auto-repeats
            int note = getNoteFromKey(keyEvent->key());
            if (note > -1)
                keyOn(note);
        }
        keyEvent->accept();
        return;
    }
    keyEvent->ignore();
}

void PianoScene::keyReleaseEvent ( QKeyEvent * keyEvent )
{
    if (m_keyboardEnabled) {
        if ( !m_rawkbd && !keyEvent->isAutoRepeat() ) { // ignore auto-repeats
            int note = getNoteFromKey(keyEvent->key());
            if (note > -1)
                keyOff(note);
        }
        keyEvent->accept();
        return;
    }
    keyEvent->ignore();
}

bool PianoScene::event(QEvent *event)
{
    switch(event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchEnd:
    case QEvent::TouchUpdate:
    {
        if (m_touchEnabled) {
            QTouchEvent *touchEvent = static_cast<QTouchEvent*>(event);
            QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
            foreach(const QTouchEvent::TouchPoint& touchPoint, touchPoints) {
                switch (touchPoint.state()) {
                //case Qt::TouchPointPrimary:
                case Qt::TouchPointStationary:
                    continue;
                case Qt::TouchPointReleased: {
                        PianoKey* key = getKeyForPos(touchPoint.scenePos());
                        if (key != NULL && key->isPressed()) {
                            keyOff(key, touchPoint.pressure());
                        }
                        break;
                    }
                case Qt::TouchPointPressed: {
                        PianoKey* key = getKeyForPos(touchPoint.scenePos());
                        if (key != NULL && !key->isPressed()) {
                            keyOn(key, touchPoint.pressure());
                            key->ensureVisible();
                        }
                        break;
                    }
                case Qt::TouchPointMoved: {
                        PianoKey* key = getKeyForPos(touchPoint.scenePos());
                        PianoKey* lastkey = getKeyForPos(touchPoint.lastScenePos());
                        if ((lastkey != NULL) && (lastkey != key) && lastkey->isPressed()) {
                            keyOff(lastkey, touchPoint.pressure());
                        }
                        if ((key != NULL) && !key->isPressed()) {
                            keyOn(key, touchPoint.pressure());
                        }
                        break;
                    }
                default:
                    //qDebug() << "TouchPoint state: " << touchPoint.state();
                    break;
                }
            }
            //qDebug() << "accepted event: " << event;
            event->accept();
            return true;
        }
        break;
    }
    default:
        break;
    }
    //qDebug() << "unprocessed event: " << event;
    return QGraphicsScene::event(event);
}

void PianoScene::allKeysOff()
{
    QList<PianoKey*>::ConstIterator it; 
    for(it = m_keys.constBegin(); it != m_keys.constEnd(); ++it) {
        (*it)->setPressed(false);
    }
}

void PianoScene::setKeyPressedColor(const QColor& color)
{
    if (color.isValid() && (color != m_keyPressedColor)) {
        m_keyPressedColor = color;
        QBrush hilightBrush(m_keyPressedColor);
        foreach(PianoKey* key, m_keys) {
            key->setPressedBrush(hilightBrush);
        }
    }
}

void PianoScene::resetKeyPressedColor()
{
    QBrush hilightBrush(m_keyPressedColor.isValid() ? m_keyPressedColor : QApplication::palette().highlight());
    foreach(PianoKey* key, m_keys) {
        key->setPressedBrush(hilightBrush);
    }
}

void PianoScene::hideOrShowKeys()
{
    QListIterator<PianoKey*> it(m_keys);
    while(it.hasNext()) {
        PianoKey* key = it.next();
        int n = m_baseOctave*12 + key->getNote() + m_transpose;
        bool b = !(n > m_maxNote) && !(n < m_minNote);
        key->setVisible(b);
    }
}

void PianoScene::setMinNote(const int note)
{
    if (m_minNote != note) {
        m_minNote = note;
        hideOrShowKeys();
    }
}

void PianoScene::setMaxNote(const int note)
{
    if (m_maxNote != note) {
        m_maxNote = note;
        hideOrShowKeys();
    }
}

void PianoScene::setBaseOctave(const int base)
{ 
    if (m_baseOctave != base) {
        m_baseOctave = base;
        hideOrShowKeys();
        refreshLabels();
    }
}

QString PianoScene::noteName(const int note)
{
    int num = (note + m_transpose + 12) % 12;
    int adj = (note + m_transpose < 0) ? 2 : 1;
    int oct = m_baseOctave + ((note + m_transpose) / 12) - adj;
    if (m_noteNames.isEmpty()) {
        QString name;
        if (!m_names_f.isEmpty() && !m_names_s.isEmpty())
            name = m_useFlats ? m_names_f.value(num) : m_names_s.value(num);
        return QString("%1<span style='vertical-align:sub;'>%2</span>").arg(name).arg(oct);
    } else {
        int noteIndex = note + m_transpose + 12*m_baseOctave;
#if defined(SMALL_SCREEN)
        return m_noteNames.value(noteIndex);
#else
        return QString("<span style='font-size:5pt;'>%1</span>").arg(m_noteNames.value(noteIndex));
#endif
    }
}

void PianoScene::refreshLabels()
{
    QListIterator<KeyLabel*> it(m_labels);
    while(it.hasNext()) {
        KeyLabel* lbl = it.next();
        PianoKey* key = dynamic_cast<PianoKey*>(lbl->parentItem());
        if(key != NULL) {
            lbl->setHtml(noteName(key->getNote()));
            lbl->setVisible(m_showLabels);
        }
    }
}

void PianoScene::refreshKeys()
{
    foreach(PianoKey* key, m_keys) {
        if (m_showColorScale && m_scalePalette != 0) {
            int degree = key->getNote() % 12;
            key->setBrush(m_scalePalette->getColor(degree));
        } else {
            key->resetBrush();
        }
        key->setPressed(false);
    }
}

void PianoScene::setShowLabels(bool show)
{
    if (m_showLabels != show) {
        m_showLabels = show;
        refreshLabels();
    }
}

void PianoScene::setUseFlats(bool use)
{
    if (m_useFlats != use) {
        m_useFlats = use;
        refreshLabels();
    }
}

void PianoScene::setTranspose(const int transpose)
{
    if (m_transpose != transpose && transpose > -12 && transpose < 12) {
        m_transpose = transpose;
        hideOrShowKeys();
        refreshLabels();
    }
}

void PianoScene::setRawKeyboardMode(bool b)
{
    if (m_rawkbd != b) {
        m_rawkbd = b;
#if defined(RAWKBD_SUPPORT)
        RawKeybdApp* rapp = dynamic_cast<RawKeybdApp*>(qApp);
        if (rapp != NULL) rapp->setRawKbdEnable(m_rawkbd);
#endif
    }
}

void PianoScene::useCustomNoteNames(const QStringList& names)
{
    m_noteNames = names;
    refreshLabels();
}

void PianoScene::useStandardNoteNames()
{
    m_noteNames.clear();
    refreshLabels();
}

void PianoScene::setKeyboardEnabled(const bool enable)
{
    if (enable != m_keyboardEnabled) {
        m_keyboardEnabled = enable;
    }
}

void PianoScene::setMouseEnabled(const bool enable)
{
    if (enable != m_mouseEnabled) {
        m_mouseEnabled = enable;
    }
}

void PianoScene::setTouchEnabled(const bool enable)
{
    if (enable != m_touchEnabled) {
        m_touchEnabled = enable;
    }
}

void PianoScene::retranslate()
{
    m_names_s.clear();
    m_names_f.clear();
    m_names_s << trUtf8("C")
              << trUtf8("C#")
              << trUtf8("D")
              << trUtf8("D#")
              << trUtf8("E")
              << trUtf8("F")
              << trUtf8("F#")
              << trUtf8("G")
              << trUtf8("G#")
              << trUtf8("A")
              << trUtf8("A#")
              << trUtf8("B");
    m_names_f << trUtf8("C")
              << trUtf8("Db")
              << trUtf8("D")
              << trUtf8("Eb")
              << trUtf8("E")
              << trUtf8("F")
              << trUtf8("Gb")
              << trUtf8("G")
              << trUtf8("Ab")
              << trUtf8("A")
              << trUtf8("Bb")
              << trUtf8("B");
    refreshLabels();
}

void PianoScene::setShowColorScale(const bool show)
{
    if (m_showColorScale != show && m_scalePalette != 0 ) {
        m_showColorScale = show;
        refreshKeys();
        invalidate();
    }
}

void PianoScene::setPianoPalette(PianoPalette *p)
{
    //qDebug() << Q_FUNC_INFO;
    resetKeyPressedColor();
    m_palette = p;
}
