/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2017, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#ifndef VPIANO_H
#define VPIANO_H

#include "ui_vpiano.h"
#include "pianoscene.h"
#if defined(RAWKBD_SUPPORT)
#include "nativefilter.h"
#endif
#include <QMainWindow>

class QTranslator;
class QLabel;
class QComboBox;
class QSpinBox;
class QSlider;
class QDial;
class Instrument;
class About;
class Preferences;
class MidiSetup;
class KMapDialog;
class DialogExtraControls;
class RiffImportDlg;
class ColorDialog;

namespace drumstick {
namespace rt {
    class MIDIInput;
    class MIDIOutput;
    class BackendManager;
}}

class VPiano : public QMainWindow, public PianoHandler
{
    Q_OBJECT

public:
    VPiano( QWidget * parent = 0, Qt::WindowFlags flags = 0 );
    virtual ~VPiano();
    bool isInitialized() const { return m_initialized; }
    void retranslateUi();
    QMenu *createPopupMenu ();

    // PianoHandler methods
    void noteOn(const int midiNote, const int vel);
    void noteOff(const int midiNote, const int vel);

    // static methods
    static QString dataDirectory();
    static QString localeDirectory();

#if ENABLE_DBUS

public Q_SLOTS:
    void quit();
    void panic();
    void reset_controllers();
    void channel(int value);
    void octave(int value);
    void transpose(int value);
    void velocity(int value);
    void connect_in(const QString &value);
    void connect_out(const QString &value);
    void connect_thru(bool value);

    void noteoff(int note);
    void noteon(int note);
    void polykeypress(int note, int value);
    void controlchange(int control, int value);
    void programchange(int value);
    void programnamechange(const QString &value);
    void chankeypress(int value);
    void pitchwheel(int value);

Q_SIGNALS:
    void event_noteoff(int note);
    void event_noteon(int note);
    void event_polykeypress(int note, int value);
    void event_controlchange(int control, int value);
    void event_programchange(int value);
    void event_chankeypress(int value);
    void event_pitchwheel(int value);

#endif /*ENABLE_DBUS*/

protected Q_SLOTS:
    void slotAbout();
    void slotAboutQt();
    void slotAboutTranslation();
    void slotConnections();
    void slotPreferences();
    void slotEditKeyboardMap();
    void slotPanic();
    void slotResetAllControllers();
    void slotResetBender();
    void slotHelpContents();
    void slotOpenWebSite();
    void slotImportSF();
    void slotEditExtraControls();
    void slotShowNoteNames();
    void slotControlSliderMoved(const int value);
    void slotExtraController(const int value);
    void slotControlClicked(const bool value);
    void slotBenderSliderMoved(const int pos);
    void slotBenderSliderReleased();
    void slotComboBankActivated(const int index = -1);
    void slotComboProgActivated(const int index = -1);
    void slotBaseOctaveValueChanged(const int octave);
    void slotTransposeValueChanged(const int transpose);
    void slotVelocityValueChanged(int value);
    void slotChannelValueChanged(const int channel);
    void slotComboControlCurrentIndexChanged(const int index);
    void slotShortcuts();
    void slotVelocityUp();
    void slotVelocityDown();
    void slotBankNext();
    void slotBankPrev();
    void slotProgramNext();
    void slotProgramPrev();
    void slotControllerNext();
    void slotControllerPrev();
    void slotControllerUp();
    void slotControllerDown();
    void slotSwitchLanguage(QAction *action);
    void slotKeyboardInput(bool value);
    void slotMouseInput(bool value);
    void slotTouchScreenInput(bool value);
    void slotColorPolicy();
    void slotColorScale(bool value);
    //void slotEditPrograms();
    //void slotDebugDestroyed(QObject *obj);

    //drumstick-rt slots
    void slotNoteOn(const int chan, const int note, const int vel);
    void slotNoteOff(const int chan, const int note, const int vel);
    void slotKeyPressure(const int chan, const int note, const int value);
    void slotController(const int chan, const int control, const int value);
    void slotProgram(const int chan, const int program);
    void slotChannelPressure(const int chan, const int value);
    void slotPitchBend(const int chan, const int value);
    void toggleWindowFrame(const bool state);

protected:
    void closeEvent ( QCloseEvent *event );
    void showEvent ( QShowEvent *event );
    void hideEvent( QHideEvent *event );

private:
    void initialization();
    bool initMidi();
    void readSettings();
    void readMidiControllerSettings();
    void writeSettings();
    void applyPreferences();
    void applyConnections();
    void applyInitialSettings();
    void applyColorPolicy();
    void populateControllers();
    void populateInstruments();
    void populatePrograms(int bank = -1);
    void refreshConnections();
    void initToolBars();
    void clearExtraControllers();
    void initExtraControllers();
    void initControllers(int channel);
    void resetAllControllers();
    void initializeAllControllers();
    void allNotesOff();
    void sendController(const int controller, const int value);
    void sendBankChange(const int bank);
    void sendNoteOn(const int midiNote, const int vel);
    void sendNoteOff(const int midiNote, const int vel);
    void sendProgramChange(const int program);
    void sendBender(const int value);
    void sendPolyKeyPress(const int note, const int value);
    void sendChanKeyPress(const int value);
    void sendSysex(const QByteArray& data);
    void updateController(int ctl, int val);
    void updateExtraController(int ctl, int val);
    void updateBankChange(int bank = -1);
    void updateProgramChange(int program = -1);
    void grabKb();
    void releaseKb();
    void updateNoteNames(bool drums);
    void setWidgetTip(QWidget* w, int val);

    QByteArray readSysexDataFile(const QString& fileName);
    void createLanguageMenu();
    QString configuredLanguage();
    void enforceMIDIChannelState();
    QColor getColorFromPolicy(const int chan, const int note, const int vel);
    int getType(const int note) const;
    int getDegree(const int note) const;
    PianoScene *currentPianoScene();

    About *dlgAbout();
    Preferences *dlgPreferences();
    MidiSetup *dlgMidiSetup();
    KMapDialog *dlgKeyMap();
    DialogExtraControls *dlgExtra();
    RiffImportDlg *dlgRiffImport();
    ColorDialog *dlgColorPolicy();

    void initLanguages();
    void retranslateToolbars();

    drumstick::rt::MIDIOutput* m_midiout;
    drumstick::rt::MIDIInput* m_midiin;
    drumstick::rt::BackendManager* m_backendManager;

    bool m_inputActive;
    bool m_midiThru;
    bool m_midiOmni;
    bool m_initialized;

    About *m_dlgAbout;
    Preferences *m_dlgPreferences;
    MidiSetup *m_dlgMidiSetup;
    KMapDialog *m_dlgKeyMap;
    DialogExtraControls *m_dlgExtra;
    RiffImportDlg *m_dlgRiffImport;
    ColorDialog *m_dlgColorPolicy;
#if defined(RAWKBD_SUPPORT)
    NativeFilter *m_filter;
#endif

    Ui::VPiano ui;

    QLabel* m_lblBank;
    QLabel* m_lblBaseOctave;
    QLabel* m_lblBender;
    QLabel* m_lblChannel;
    QLabel* m_lblControl;
    QLabel* m_lblProgram;
    QLabel* m_lblTranspose;
    QLabel* m_lblValue;
    QLabel* m_lblVelocity;
    QSpinBox* m_sboxChannel;
    QSpinBox* m_sboxOctave;
    QSpinBox* m_sboxTranspose;
    QDial* m_Velocity;
    QComboBox* m_comboControl;
    QDial* m_Control;
    QSlider* m_bender;
    QComboBox* m_comboBank;
    QComboBox* m_comboProg;
    Instrument* m_ins;
    QStringList m_extraControls;
    QMap<int,QMap<int,int> > m_ctlState, m_ctlSettings;
    QMap<int,int> m_lastBank;
    QMap<int,int> m_lastProg;
    QMap<int,int> m_lastCtl;
    int m_baseChannel;
    int m_velocity;
    int m_baseOctave;
    int m_transpose;
    QString m_language;
    QMap<QString, QString> m_supportedLangs;
    QTranslator *m_trq, *m_trp;
    QAction *m_currentLang;
    QHash<QString,QList<QKeySequence> > m_defaultShortcuts;
    int m_currentPalette;

    QString m_lastInputBackend;
    QString m_lastOutputBackend;
    QString m_lastInputConnection;
    QString m_lastOutputConnection;
    QString m_defaultInputBackend;
    QString m_defaultOutputBackend;
    QString m_defaultInputConnection;
    QString m_defaultOutputConnection;
    bool m_advanced;
    bool m_inputEnabled;
};

#endif // VPIANO_H
