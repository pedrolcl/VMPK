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

#ifndef PIANOKEYBD_H
#define PIANOKEYBD_H

#include "pianoscene.h"
#if defined(RAWKBD_SUPPORT)
#include "rawkeybdapp.h"
#endif
#include <QGraphicsView>

#if defined(VPIANO_PLUGIN)
#include <QtDesigner/QDesignerExportWidget>
#else
#define QDESIGNER_WIDGET_EXPORT
#endif

class QDESIGNER_WIDGET_EXPORT PianoKeybd : public QGraphicsView
#if defined(RAWKBD_SUPPORT)
        , public RawKbdHandler
#endif
{
    Q_OBJECT
    Q_PROPERTY( int baseOctave READ baseOctave WRITE setBaseOctave )
    Q_PROPERTY( int numOctaves READ numOctaves WRITE setNumOctaves )
    Q_PROPERTY( int rotation READ getRotation WRITE setRotation )
    Q_PROPERTY( QColor keyPressedColor READ getKeyPressedColor WRITE setKeyPressedColor )
    Q_PROPERTY( bool showLabels READ showLabels WRITE setShowLabels )
    Q_PROPERTY( bool useFlats READ useFlats WRITE setUseFlats )
    Q_PROPERTY( int transpose READ getTranspose WRITE setTranspose )
#if defined(VPIANO_PLUGIN)
    Q_CLASSINFO("Author", "Pedro Lopez-Cabanillas <plcl@users.sf.net>")
    Q_CLASSINFO("URL", "http://sourceforge.net/projects/vmpk")
    Q_CLASSINFO("Version", "0.10")
#endif
public:
    PianoKeybd(QWidget *parent = 0);
    PianoKeybd(const int baseOctave, const int numOctaves, QWidget *parent = 0);
    virtual ~PianoKeybd();

    int baseOctave() const { return m_scene->baseOctave(); }
    void setBaseOctave(const int baseOctave) { m_scene->setBaseOctave(baseOctave); }
    int numOctaves() const { return m_scene->numOctaves(); }
    void setNumOctaves(const int numOctaves);
    int getRotation() const { return m_rotation; }
    void setRotation(int r);
    QColor getKeyPressedColor() const { return m_scene->getKeyPressedColor(); }
    void setKeyPressedColor(const QColor& c) { m_scene->setKeyPressedColor(c); }
    bool showLabels() const { return m_scene->showLabels(); }
    void setShowLabels(bool show) { m_scene->setShowLabels(show); }
    bool useFlats() const { return m_scene->useFlats(); }
    void setUseFlats(bool use) { m_scene->setUseFlats(use); }
    int getTranspose() const { return m_scene->getTranspose(); }
    void setTranspose(int t) { m_scene->setTranspose(t); }

    QSize sizeHint() const;
    PianoScene* getPianoScene() { return m_scene; }
    void setRawKeyboardMap(KeyboardMap* m) { m_rawMap = m; }
    KeyboardMap* getRawKeyboardMap() { return m_rawMap; }
    void resetRawKeyboardMap() { m_rawMap = &m_defaultRawMap; }
    void resetKeyboardMap() { m_scene->setKeyboardMap(&m_defaultMap); }

#if defined(RAWKBD_SUPPORT)
// RawKbdHandler methods
    bool handleKeyPressed(int keycode);
    bool handleKeyReleased(int keycode);
#endif

signals:
    void noteOn( int midiNote, int vel );
    void noteOff( int midiNote, int vel );

protected:
    void initialize();
    void initDefaultMap();
    void initScene(int base, int num, const QColor& c = QColor());
    void resizeEvent(QResizeEvent *event);

private:
    int m_rotation;
    PianoScene *m_scene;
    KeyboardMap *m_rawMap;
    KeyboardMap m_defaultMap;
    KeyboardMap m_defaultRawMap;
};

#endif // PIANOKEYBD_H
