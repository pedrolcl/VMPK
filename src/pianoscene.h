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

#ifndef PIANOSCENE_H_
#define PIANOSCENE_H_

#include "pianokey.h"
#include "keylabel.h"
#include "keyboardmap.h"
#include "pianopalette.h"

#include <QGraphicsScene>
#include <QHash>

class PianoHandler
{
public:
    virtual ~PianoHandler() {}
    virtual void noteOn( const int note, const int vel ) = 0;
    virtual void noteOff( const int note, const int vel ) = 0;
};

class VPIANO_EXPORT PianoScene : public QGraphicsScene
{
    Q_OBJECT
    
public:
    PianoScene ( const int baseOctave, 
                 const int numOctaves,
                 const QColor& keyPressedColor = QColor(),
                 QObject * parent = 0 );
    
    QSize sizeHint() const;
    void setKeyboardMap( KeyboardMap* map ) { m_keybdMap = map; }
    KeyboardMap* getKeyboardMap() const { return m_keybdMap; }
    PianoHandler* getPianoHandler() const { return m_handler; }
    void setPianoHandler(PianoHandler* handler) { m_handler = handler; }
    PianoPalette* getPianoPalette() const { return m_palette; }
    void setPianoPalette( PianoPalette* p );
    void setColorScalePalette( PianoPalette* p ) { m_scalePalette = p; }

    QColor getKeyPressedColor() const { return m_keyPressedColor; }
    void setKeyPressedColor(const QColor& color);
    void resetKeyPressedColor();
    int getMinNote() const { return m_minNote; } 
    void setMinNote(const int note);
    int getMaxNote() const { return m_maxNote; }
    void setMaxNote(const int note);
    int getTranspose() const { return m_transpose; }
    void setTranspose(const int transpose);
    bool showLabels() const { return m_showLabels; }
    void setShowLabels(const bool show);
    bool useFlats() const { return m_useFlats; }
    void setUseFlats(const bool use);
    bool isKeyboardEnabled() const { return m_keyboardEnabled; }
    void setKeyboardEnabled( const bool enable );
    bool isMouseEnabled() const { return m_mouseEnabled; }
    void setMouseEnabled( const bool enable );
    bool isTouchEnabled() const { return m_touchEnabled; }
    void setTouchEnabled( const bool enable );
    bool showColorScale() const { return m_showColorScale; }
    void setShowColorScale(const bool show);

    void showNoteOn( const int note, QColor color, int vel = -1 );
    void showNoteOn( const int note, int vel = -1 );
    void showNoteOff( const int note, int vel = -1 );
    int baseOctave() const { return m_baseOctave; }
    void setBaseOctave( const int base );
    int numOctaves() const { return m_numOctaves; }
    void allKeysOff();
    void keyOn( const int note );
    void keyOff( const int note );
    bool getRawKeyboardMode() const { return m_rawkbd; }
    void setRawKeyboardMode(const bool b);
    void useCustomNoteNames(const QStringList& names);
    void useStandardNoteNames();
    int getVelocity() { return m_velocity; }
    void setVelocity(const int velocity) { m_velocity = velocity; }
    int getChannel() const { return m_channel; }
    void setChannel(const int channel) { m_channel = channel; }
    void retranslate();

signals:
    void noteOn(int n, int v);
    void noteOff(int n, int v);

protected:
    void showKeyOn( PianoKey* key, QColor color, int vel );
    void showKeyOn( PianoKey* key, int vel );
    void showKeyOff( PianoKey* key, int vel );
    void keyOn( PianoKey* key );
    void keyOff( PianoKey* key );
    void keyOn( PianoKey* key, qreal pressure );
    void keyOff( PianoKey* key, qreal pressure );
    PianoKey* getKeyForPos( const QPointF& p ) const;
    PianoKey* getPianoKey( const int key ) const;
    QString noteName(const int note);
    void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    void keyPressEvent ( QKeyEvent * keyEvent );
    void keyReleaseEvent ( QKeyEvent * keyEvent );
    bool event(QEvent *event);

private:
    void hideOrShowKeys();
    void refreshLabels();
    void refreshKeys();
    void triggerNoteOn( const int note, const int vel );
    void triggerNoteOff( const int note, const int vel );
    int getNoteFromKey( const int key ) const;
    void setColorFromPolicy(PianoKey* key, const int vel);

    int m_baseOctave;
    int m_numOctaves;
    int m_minNote;
    int m_maxNote;
    int m_transpose;
    bool m_showLabels;
    bool m_useFlats;
    bool m_rawkbd;
    bool m_keyboardEnabled;
    bool m_mouseEnabled;
    bool m_touchEnabled;
    QColor m_keyPressedColor;
    bool m_mousePressed;
    int m_velocity;
    int m_channel;
    bool m_showColorScale;
    PianoHandler* m_handler;
    KeyboardMap* m_keybdMap;
    QList<PianoKey*> m_keys;
    QList<KeyLabel*> m_labels;
    QStringList m_noteNames;
    QStringList m_names_s;
    QStringList m_names_f;
    PianoPalette* m_palette;
    PianoPalette* m_scalePalette;
};

#endif /*PIANOSCENE_H_*/
