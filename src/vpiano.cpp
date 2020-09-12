/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2020, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <QDesktopServices>
#include <QInputDialog>
#include <QDir>
#include <QMessageBox>
#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QSlider>
#include <QSpinBox>
#include <QDial>
#include <QToolButton>
#include <QToolTip>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QDialogButtonBox>
#include <QDialog>
#include <QUrl>
#include <QString>
#include <QTranslator>
#include <QLibraryInfo>
#include <QMapIterator>
#include <QShortcut>
#include <QDebug>

#include <drumstick/backendmanager.h>
#include <drumstick/rtmidiinput.h>
#include <drumstick/rtmidioutput.h>
#include <drumstick/pianokeybd.h>
#include <drumstick/settingsfactory.h>

#include "vpiano.h"
#include "instrument.h"
#include "mididefs.h"
#include "constants.h"
#include "riffimportdlg.h"
#include "extracontrols.h"
#include "about.h"
#include "preferences.h"
#include "midisetup.h"
#include "colordialog.h"
#include "vpianosettings.h"

#if !defined(SMALL_SCREEN)
#include "kmapdialog.h"
#include "shortcutdialog.h"
#endif

#if defined(ENABLE_DBUS)
#include "vmpk_adaptor.h"
#include <QtDBus/QDBusConnection>
#endif

#if defined(Q_OS_MACOS)
#include <CoreFoundation/CoreFoundation.h>
#endif

using namespace drumstick::rt;
using namespace drumstick::widgets;

VPiano::VPiano( QWidget * parent, Qt::WindowFlags flags )
    : QMainWindow(parent, flags),
    m_midiout(nullptr),
    m_midiin(nullptr),
    m_backendManager(nullptr),
    m_initialized(false),
    m_filter(nullptr)
{
#if defined(ENABLE_DBUS)
    new VmpkAdaptor(this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject("/", this);
    dbus.registerService("net.sourceforge.vmpk");
#endif
    m_trq = new QTranslator(this);
    m_trp = new QTranslator(this);
    //qDebug() << QSTR_QTPX + configuredLanguage() << QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    //qDebug() << QSTR_VMPKPX + configuredLanguage() << VPiano::localeDirectory();
    if (!m_trq->load( QSTR_QTPX + configuredLanguage(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath) )) {
        qWarning() << "Failure loading Qt5 translations for" << configuredLanguage()
                   << "from" << QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    }
    if (!m_trp->load( QSTR_VMPKPX + configuredLanguage(),
                      VPiano::localeDirectory() )) {
        qWarning() << "Failure loading VMPK translations for" << configuredLanguage()
                   << "from" << VPiano::localeDirectory();
    }
    QCoreApplication::installTranslator(m_trq);
    QCoreApplication::installTranslator(m_trp);
    ui.setupUi(this);
    initLanguages();

    QActionGroup* nameVisibilityGroup = new QActionGroup(this);
    nameVisibilityGroup->setExclusive(true);
    nameVisibilityGroup->addAction(ui.actionNever);
    nameVisibilityGroup->addAction(ui.actionMinimal);
    nameVisibilityGroup->addAction(ui.actionWhen_Activated);
    nameVisibilityGroup->addAction(ui.actionAlways);
    connect(nameVisibilityGroup, &QActionGroup::triggered, this, &VPiano::slotNameVisibility);

    QActionGroup* blackKeysGroup = new QActionGroup(this);
    blackKeysGroup->setExclusive(true);
    blackKeysGroup->addAction(ui.actionFlats);
    blackKeysGroup->addAction(ui.actionSharps);
    blackKeysGroup->addAction(ui.actionNothing);
    connect(blackKeysGroup, &QActionGroup::triggered, this, &VPiano::slotNameVariant);

    QActionGroup* orientationGroup = new QActionGroup(this);
    orientationGroup->setExclusive(true);
    orientationGroup->addAction(ui.actionHorizontal);
    orientationGroup->addAction(ui.actionVertical);
    orientationGroup->addAction(ui.actionAutomatic);
    connect(orientationGroup, &QActionGroup::triggered, this, &VPiano::slotNameOrientation);

    connect(ui.pianokeybd, &PianoKeybd::signalName, this, &VPiano::slotNoteName);
    connect(ui.actionAbout, SIGNAL(triggered()), SLOT(slotAbout()));
    connect(ui.actionAboutQt, SIGNAL(triggered()), SLOT(slotAboutQt()));
    connect(ui.actionAboutTranslation, SIGNAL(triggered()), SLOT(slotAboutTranslation()));
    connect(ui.actionConnections, SIGNAL(triggered()), SLOT(slotConnections()));
    connect(ui.actionPreferences, SIGNAL(triggered()), SLOT(slotPreferences()));
    connect(ui.actionEditKM, SIGNAL(triggered()), SLOT(slotEditKeyboardMap()));
    connect(ui.actionContents, SIGNAL(triggered()), SLOT(slotHelpContents()));
    connect(ui.actionWebSite, SIGNAL(triggered()), SLOT(slotOpenWebSite()));
    connect(ui.actionImportSoundFont, SIGNAL(triggered()), SLOT(slotImportSF()));
    connect(ui.actionEditExtraControls, SIGNAL(triggered()), SLOT(slotEditExtraControls()));
    connect(ui.actionShortcuts, SIGNAL(triggered()), SLOT(slotShortcuts()));
    connect(ui.actionKeyboardInput, SIGNAL(toggled(bool)), SLOT(slotKeyboardInput(bool)));
    connect(ui.actionMouseInput, SIGNAL(toggled(bool)), SLOT(slotMouseInput(bool)));
    connect(ui.actionTouchScreenInput, SIGNAL(toggled(bool)), SLOT(slotTouchScreenInput(bool)));
    connect(ui.actionColorPalette, SIGNAL(triggered()), SLOT(slotColorPolicy()));
    connect(ui.actionColorScale, SIGNAL(toggled(bool)), SLOT(slotColorScale(bool)));
    connect(ui.actionWindowFrame, SIGNAL(toggled(bool)), SLOT(toggleWindowFrame(bool)));
    // Toolbars actions: toggle view
    connect(ui.toolBarNotes->toggleViewAction(), SIGNAL(toggled(bool)),
            ui.actionNotes, SLOT(setChecked(bool)));
    connect(ui.toolBarControllers->toggleViewAction(), SIGNAL(toggled(bool)),
            ui.actionControllers, SLOT(setChecked(bool)));
    connect(ui.toolBarBender->toggleViewAction(), SIGNAL(toggled(bool)),
            ui.actionBender, SLOT(setChecked(bool)));
    connect(ui.toolBarPrograms->toggleViewAction(), SIGNAL(toggled(bool)),
            ui.actionPrograms, SLOT(setChecked(bool)));
    connect(ui.toolBarExtra->toggleViewAction(), SIGNAL(toggled(bool)),
            ui.actionExtraControls, SLOT(setChecked(bool)));
#if defined(SMALL_SCREEN)
    ui.toolBarControllers->hide();
    ui.toolBarBender->hide();
    ui.toolBarExtra->hide();
    //ui.toolBarNotes->hide();
    //ui.toolBarPrograms->hide();
    ui.actionEditKM->setVisible(false);
    ui.actionShortcuts->setVisible(false);
    ui.actionStatusBar->setVisible(false);
    setWindowTitle("VMPK " + PGM_VERSION);
#endif
#if defined (Q_OS_MACX)
    //ui.actionEnterFullScreen->setShortcut(QKeySequence::FullScreen);
    //ui.actionExitFullScreen->setShortcut(QKeySequence::FullScreen);
#endif
    ui.pianokeybd->setPianoHandler(this);
#if defined(RAWKBD_SUPPORT)
    m_filter = new NativeFilter;
    m_filter->setRawKbdHandler(ui.pianokeybd);
    qApp->installNativeEventFilter(m_filter);
#endif
    initialization();
}

VPiano::~VPiano()
{
#if defined(RAWKBD_SUPPORT)
    m_filter->setRawKbdEnabled(false);
    qApp->removeNativeEventFilter(m_filter);
    delete m_filter;
#endif
    delete m_backendManager;
}

void VPiano::initialization()
{
    readSettings();
    if ((m_initialized = initMidi())) {
        readMidiControllerSettings();
        createLanguageMenu();
        initToolBars();
        applyPreferences();
        applyInitialSettings();
        initExtraControllers();
        enforceMIDIChannelState();
        activateWindow();
    } else {
        qWarning() << "Unable to initialize all MIDI drivers. Terminating.";
    }
}

bool VPiano::initMidi()
{
    m_backendManager = new BackendManager();
    m_backendManager->refresh(VPianoSettings::instance()->settingsMap());

    m_midiin = m_backendManager->inputBackendByName(VPianoSettings::instance()->lastInputBackend());
    if (m_midiin == nullptr) {
        if (VPianoSettings::instance()->lastInputBackend() != VPianoSettings::instance()->nativeInput()) {
            qWarning() << "Last MIDI IN driver" << VPianoSettings::instance()->lastInputBackend() << "not found";
        }
        m_midiin = m_backendManager->inputBackendByName(VPianoSettings::instance()->nativeInput());
        if (m_midiin == nullptr) {
            qWarning() << "Default MIDI IN driver" << VPianoSettings::instance()->nativeInput() << "not found";
        }
    }

    m_midiout = m_backendManager->outputBackendByName(VPianoSettings::instance()->lastOutputBackend());
    if (m_midiout == nullptr) {
        if (VPianoSettings::instance()->lastOutputBackend() != VPianoSettings::instance()->nativeOutput()) {
            qWarning() << "Last MIDI OUT driver" << VPianoSettings::instance()->lastOutputBackend() << "not found";
        }
        m_midiout = m_backendManager->outputBackendByName(VPianoSettings::instance()->nativeOutput());
        if (m_midiout == nullptr) {
            qWarning() << "Default MIDI OUT driver" << VPianoSettings::instance()->nativeOutput() << "not found";
        }
    }

    SettingsFactory settings;

    if (m_midiin != nullptr) {
        if (!VPianoSettings::instance()->lastInputConnection().isEmpty()) {
            m_midiin->initialize(settings.getQSettings());
            for(const MIDIConnection& conn : m_midiin->connections()) {
                if (conn.first == VPianoSettings::instance()->lastInputConnection()) {
                    m_midiin->open(conn);
                    break;
                }
            }
        }
    }

    if (m_midiout != nullptr && !VPianoSettings::instance()->lastOutputConnection().isEmpty()) {
        m_midiout->initialize(settings.getQSettings());
        for(const MIDIConnection& conn : m_midiout->connections()) {
            if (conn.first == VPianoSettings::instance()->lastOutputConnection()) {
                m_midiout->open(conn);
                if (!VPianoSettings::instance()->lastInputConnection().isEmpty()) {
                    m_midiin->setMIDIThruDevice(m_midiout);
                    m_midiin->enableMIDIThru(VPianoSettings::instance()->midiThru());
                }
                break;
            }
        }
    }

    return (m_midiout != nullptr);
}

void VPiano::initToolBars()
{
    // Notes tool bar
    QWidget *w = ui.toolBarNotes->widgetForAction(ui.actionPanic);
    w->setMaximumWidth(120);
    m_lblChannel = new QLabel(this);
    ui.toolBarNotes->addWidget(m_lblChannel);
    m_lblChannel->setMargin(TOOLBARLABELMARGIN);
    m_sboxChannel = new QSpinBox(this);
    m_sboxChannel->setMinimum(1);
    m_sboxChannel->setMaximum(MIDICHANNELS);
    m_sboxChannel->setValue(VPianoSettings::instance()->channel() + 1);
    m_sboxChannel->setFocusPolicy(Qt::NoFocus);
    ui.toolBarNotes->addWidget(m_sboxChannel);
    m_lblBaseOctave = new QLabel(this);
    ui.toolBarNotes->addWidget(m_lblBaseOctave);
    m_lblBaseOctave->setMargin(TOOLBARLABELMARGIN);
    m_sboxOctave = new QSpinBox(this);
    m_sboxOctave->setMinimum(0);
    m_sboxOctave->setMaximum(9);
    m_sboxOctave->setValue(VPianoSettings::instance()->baseOctave());
    m_sboxOctave->setFocusPolicy(Qt::NoFocus);
    ui.toolBarNotes->addWidget(m_sboxOctave);
    m_lblTranspose = new QLabel(this);
    ui.toolBarNotes->addWidget(m_lblTranspose);
    m_lblTranspose->setMargin(TOOLBARLABELMARGIN);
    m_sboxTranspose = new QSpinBox(this);
    m_sboxTranspose->setMinimum(-11);
    m_sboxTranspose->setMaximum(11);
    m_sboxTranspose->setValue(VPianoSettings::instance()->transpose());
    m_sboxTranspose->setFocusPolicy(Qt::NoFocus);
    ui.toolBarNotes->addWidget(m_sboxTranspose);
    m_lblVelocity = new QLabel(this);
    ui.toolBarNotes->addWidget(m_lblVelocity);
    m_lblVelocity->setMargin(TOOLBARLABELMARGIN);
    m_Velocity = new QDial(this);
    m_Velocity->setFixedSize(32, 32);
    m_Velocity->setMinimum(0);
    m_Velocity->setMaximum(127);
    m_Velocity->setValue(VPianoSettings::instance()->velocity());
    m_Velocity->setToolTip(QString::number(VPianoSettings::instance()->velocity()));
    m_Velocity->setFocusPolicy(Qt::NoFocus);
    ui.toolBarNotes->addWidget(m_Velocity);
    connect( m_sboxChannel, SIGNAL(valueChanged(int)),
             SLOT(slotChannelValueChanged(int)));
    connect( m_sboxOctave, SIGNAL(valueChanged(int)),
             SLOT(slotBaseOctaveValueChanged(int)) );
    connect( m_sboxTranspose, SIGNAL(valueChanged(int)),
             SLOT(slotTransposeValueChanged(int)) );
    connect( m_Velocity, SIGNAL(valueChanged(int)),
             SLOT(slotVelocityValueChanged(int)) );
    connect( ui.actionChannelUp, SIGNAL(triggered()),
             m_sboxChannel, SLOT(stepUp()) );
    connect( ui.actionChannelDown, SIGNAL(triggered()),
             m_sboxChannel, SLOT(stepDown()) );
    connect( ui.actionOctaveUp, SIGNAL(triggered()),
             m_sboxOctave, SLOT(stepUp()) );
    connect( ui.actionOctaveDown, SIGNAL(triggered()),
             m_sboxOctave, SLOT(stepDown()) );
    connect( ui.actionTransposeUp, SIGNAL(triggered()),
             m_sboxTranspose, SLOT(stepUp()) );
    connect( ui.actionTransposeDown, SIGNAL(triggered()),
             m_sboxTranspose, SLOT(stepDown()) );
    connect( ui.actionVelocityUp, SIGNAL(triggered()),
             SLOT(slotVelocityUp()) );
    connect( ui.actionVelocityDown, SIGNAL(triggered()),
             SLOT(slotVelocityDown()) );
    // Controllers tool bar
    m_lblControl = new QLabel(this);
    ui.toolBarControllers->addWidget(m_lblControl);
    m_lblControl ->setMargin(TOOLBARLABELMARGIN);
    m_comboControl = new QComboBox(this);
    m_comboControl->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_comboControl->setFocusPolicy(Qt::NoFocus);
    ui.toolBarControllers->addWidget(m_comboControl);
    m_lblValue = new QLabel(this);
    ui.toolBarControllers->addWidget(m_lblValue);
    m_lblValue->setMargin(TOOLBARLABELMARGIN);
    m_Control= new QDial(this);
    m_Control->setFixedSize(32, 32);
    m_Control->setMinimum(0);
    m_Control->setMaximum(127);
    m_Control->setValue(0);
    m_Control->setToolTip("0");
    m_Control->setFocusPolicy(Qt::NoFocus);
    ui.toolBarControllers->addWidget(m_Control);
    connect( m_comboControl, SIGNAL(currentIndexChanged(int)),
             SLOT(slotComboControlCurrentIndexChanged(int)) );
    connect( m_Control, SIGNAL(sliderMoved(int)),
             SLOT(slotControlSliderMoved(int)) );
    // Pitch bender tool bar
    m_lblBender = new QLabel(this);
    ui.toolBarBender->addWidget(m_lblBender);
    m_lblBender->setMargin(TOOLBARLABELMARGIN);
    m_bender = new QSlider(this);
    m_bender->setOrientation(Qt::Horizontal);
    m_bender->setMaximumWidth(200);
    m_bender->setMinimum(BENDER_MIN);
    m_bender->setMaximum(BENDER_MAX);
    m_bender->setValue(0);
    m_bender->setToolTip("0");
    m_bender->setFocusPolicy(Qt::NoFocus);
    ui.toolBarBender->addWidget(m_bender);
    connect( m_bender, SIGNAL(sliderMoved(int)),
             SLOT(slotBenderSliderMoved(int)) );
    connect( m_bender, SIGNAL(sliderReleased()),
             SLOT(slotBenderSliderReleased()) );
    // Programs tool bar
    m_lblBank = new QLabel(this);
    ui.toolBarPrograms->addWidget(m_lblBank);
    m_lblBank->setMargin(TOOLBARLABELMARGIN);
    m_comboBank = new QComboBox(this);
    m_comboBank->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_comboBank->setFocusPolicy(Qt::NoFocus);
    ui.toolBarPrograms->addWidget(m_comboBank);
    m_lblProgram = new QLabel(this);
    ui.toolBarPrograms->addWidget(m_lblProgram);
    m_lblProgram->setMargin(TOOLBARLABELMARGIN);
    m_comboProg = new QComboBox(this);
    m_comboProg->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_comboProg->setFocusPolicy(Qt::NoFocus);
    ui.toolBarPrograms->addWidget(m_comboProg);
    connect( m_comboBank, SIGNAL(activated(int)),
             SLOT(slotComboBankActivated(int)) );
    connect( m_comboProg, SIGNAL(activated(int)),
             SLOT(slotComboProgActivated(int)) );
    // Toolbars actions: buttons
    connect( ui.actionPanic, SIGNAL(triggered()),
             SLOT(slotPanic()));
    connect( ui.actionResetAll, SIGNAL(triggered()),
             SLOT(slotResetAllControllers()));
    connect( ui.actionReset, SIGNAL(triggered()),
             SLOT(slotResetBender()));
    connect( ui.actionEditExtra, SIGNAL(triggered()),
             SLOT(slotEditExtraControls()));
    // Tools actions
    connect( ui.actionNextBank, SIGNAL(triggered()),
             SLOT(slotBankNext()) );
    connect( ui.actionPreviousBank, SIGNAL(triggered()),
             SLOT(slotBankPrev()) );
    connect( ui.actionNextProgram, SIGNAL(triggered()),
             SLOT(slotProgramNext()) );
    connect( ui.actionPreviousProgram, SIGNAL(triggered()),
             SLOT(slotProgramPrev()) );
    connect( ui.actionNextController, SIGNAL(triggered()),
             SLOT(slotControllerNext()) );
    connect( ui.actionPreviousController, SIGNAL(triggered()),
             SLOT(slotControllerPrev()) );
    connect( ui.actionControllerDown, SIGNAL(triggered()),
             SLOT(slotControllerDown()) );
    connect( ui.actionControllerUp, SIGNAL(triggered()),
             SLOT(slotControllerUp()) );
    /* connect( ui.actionEditPrograms, SIGNAL(triggered()),
             SLOT(slotEditPrograms())); */
    retranslateToolbars();
}

//void VPiano::slotDebugDestroyed(QObject *obj)
//{
//    qDebug() << Q_FUNC_INFO << obj->metaObject()->className();
//}

void VPiano::clearExtraControllers()
{
    QList<QAction*> allActs = ui.toolBarExtra->actions();
    foreach(QAction* a, allActs) {
        if (a != ui.actionEditExtra) {
            ui.toolBarExtra->removeAction(a);
            delete a;
        }
    }
    ui.toolBarExtra->clear();
    ui.toolBarExtra->addAction(ui.actionEditExtra);
    ui.toolBarExtra->addSeparator();
}

QByteArray VPiano::readSysexDataFile(const QString& fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QByteArray res = file.readAll();
    file.close();
    return res;
}

void VPiano::initExtraControllers()
{
    QWidget *w = nullptr;
    QCheckBox *chkbox = nullptr;
    QDial *knob = nullptr;
    QSpinBox *spin = nullptr;
    QSlider *slider = nullptr;
    QToolButton *button = nullptr;
    foreach(const QString& s, m_extraControls) {
        QString lbl;
        int control = 0;
        int type = 0;
        int minValue = 0;
        int maxValue = 127;
        int defValue = 0;
        int value = 0;
        int size = 100;
        int channel = VPianoSettings::instance()->channel();
        QString fileName;
        QString keySequence;
        ExtraControl::decodeString( s, lbl, control, type,
                                    minValue, maxValue, defValue,
                                    size, fileName, keySequence );
        if (m_ctlState[channel].contains(control))
            value = m_ctlState[channel][control];
        else
            value = defValue;
        switch(type) {
        case ExtraControl::ControlType::SwitchControl:
            chkbox = new QCheckBox(this);
            chkbox->setStyleSheet(QSTR_CHKBOXSTYLE);
            chkbox->setProperty(MIDICTLONVALUE, maxValue);
            chkbox->setProperty(MIDICTLOFFVALUE, minValue);
            chkbox->setChecked(bool(value));
            connect(chkbox, SIGNAL(clicked(bool)), SLOT(slotControlClicked(bool)));
            if (!keySequence.isEmpty()) {
                QShortcut *s = new QShortcut(QKeySequence(keySequence), chkbox, SLOT(click()));
                s->setAutoRepeat(false);
            }
            w = chkbox;
            break;
        case ExtraControl::ControlType::KnobControl:
            knob = new QDial(this);
            knob->setFixedSize(32, 32);
            knob->setMinimum(minValue);
            knob->setMaximum(maxValue);
            knob->setValue(value);
            knob->setToolTip(QString::number(value));
            connect(knob, SIGNAL(sliderMoved(int)), SLOT(slotExtraController(int)));
            w = knob;
            break;
        case ExtraControl::ControlType::SpinBoxControl:
            spin = new QSpinBox(this);
            spin->setMinimum(minValue);
            spin->setMaximum(maxValue);
            spin->setValue(value);
            connect(spin, SIGNAL(valueChanged(int)), SLOT(slotExtraController(int)));
            w = spin;
            break;
        case ExtraControl::ControlType::SliderControl:
            slider = new QSlider(this);
            slider->setOrientation(Qt::Horizontal);
            slider->setFixedWidth(size);
            slider->setMinimum(minValue);
            slider->setMaximum(maxValue);
            slider->setToolTip(QString::number(value));
            slider->setValue(value);
            connect(slider, SIGNAL(sliderMoved(int)), SLOT(slotExtraController(int)));
            w = slider;
            break;
        case ExtraControl::ControlType::ButtonCtlControl:
            button = new QToolButton(this);
            button->setText(lbl);
            button->setProperty(MIDICTLONVALUE, maxValue);
            button->setProperty(MIDICTLOFFVALUE, minValue);
            connect(button, SIGNAL(clicked(bool)), SLOT(slotControlClicked(bool)));
            if (!keySequence.isEmpty()) {
                QShortcut *s = new QShortcut(QKeySequence(keySequence), button, SLOT(animateClick()));
                s->setAutoRepeat(false);
            }
            w = button;
            break;
        case ExtraControl::ControlType::ButtonSyxControl:
            control = 255;
            button = new QToolButton(this);
            button->setText(lbl);
            button->setProperty(SYSEXFILENAME, fileName);
            button->setProperty(SYSEXFILEDATA, readSysexDataFile(fileName));
            connect(button, SIGNAL(clicked(bool)), SLOT(slotControlClicked(bool)));
            if (!keySequence.isEmpty()) {
                QShortcut *s = new QShortcut(QKeySequence(keySequence), button, SLOT(animateClick()));
                s->setAutoRepeat(false);
            }
            w = button;
            break;
        default:
            w = nullptr;
        }
        if (w != nullptr) {
            if (!lbl.isEmpty() && type < 4) {
                QLabel *qlbl = new QLabel(lbl, this);
                qlbl->setMargin(TOOLBARLABELMARGIN);
                ui.toolBarExtra->addWidget(qlbl);
                //connect(qlbl, SIGNAL(destroyed(QObject*)), SLOT(slotDebugDestroyed(QObject*)));
            }
            w->setProperty(MIDICTLNUMBER, control);
            w->setFocusPolicy(Qt::NoFocus);
            ui.toolBarExtra->addWidget(w);
            //connect(w, SIGNAL(destroyed(QObject*)), SLOT(slotDebugDestroyed(QObject*)));
        }
    }
}

void VPiano::readSettings()
{
    VPianoSettings::instance()->ReadSettings();
    restoreGeometry(VPianoSettings::instance()->geometry());
    restoreState(VPianoSettings::instance()->state());

    ui.actionStatusBar->setChecked(VPianoSettings::instance()->showStatusBar());
    ui.pianokeybd->setNumKeys(VPianoSettings::instance()->numKeys(), VPianoSettings::instance()->startingKey());
    ui.pianokeybd->setVelocityTint(VPianoSettings::instance()->velocityColor());
    ui.pianokeybd->setVelocity(VPianoSettings::instance()->velocity());
    ui.pianokeybd->setTranspose(VPianoSettings::instance()->transpose());
    ui.pianokeybd->setBaseOctave(VPianoSettings::instance()->baseOctave());
    ui.pianokeybd->setKeyboardEnabled(VPianoSettings::instance()->enableKeyboard());
    ui.pianokeybd->setMouseEnabled(VPianoSettings::instance()->enableMouse());
    ui.pianokeybd->setTouchEnabled(VPianoSettings::instance()->enableTouch());
    ui.pianokeybd->setChannel(VPianoSettings::instance()->channel());
    ui.actionColorScale->setChecked(VPianoSettings::instance()->colorScale());
    ui.pianokeybd->setFont(VPianoSettings::instance()->namesFont());
    ui.pianokeybd->setShowLabels(VPianoSettings::instance()->namesVisibility());

    QString insFileName = VPianoSettings::instance()->insFileName();
    if (!insFileName.isEmpty()) {
        VPianoSettings::instance()->setInstrumentsFileName(insFileName);
    }

    bool savedShortcuts = VPianoSettings::instance()->savedShortcuts();
    QList<QAction *> actions = findChildren<QAction *> ();
    foreach(QAction* pAction, actions)
    {
        if (pAction->objectName().isEmpty())
            continue;
        const QString& sKey = pAction->objectName();
        QList<QKeySequence> sShortcuts = pAction->shortcuts();
        m_defaultShortcuts.insert(sKey, sShortcuts);
        if (savedShortcuts)
        {
            const QString& sValue = VPianoSettings::instance()->getShortcut(sKey);
            if (sValue.isEmpty())
            {
                if(sShortcuts.count() == 0)
                {
                    continue;
                }
                else
                {
                    pAction->setShortcuts(QList<QKeySequence>());
                }
            }
            else
            {
                pAction->setShortcut(QKeySequence(sValue));
            }
        }
    }

    QString mapFile = VPianoSettings::instance()->getMapFile();
    QString rawMapFile = VPianoSettings::instance()->getRawMapFile();
    if (!mapFile.isEmpty() && mapFile != QSTR_DEFAULT) {
        ui.pianokeybd->setKeyboardMap(VPianoSettings::instance()->getKeyboardMap());
    }
    if (!rawMapFile.isEmpty() && rawMapFile != QSTR_DEFAULT) {
        ui.pianokeybd->setRawKeyboardMap(VPianoSettings::instance()->getRawKeyboardMap());
    }
}

void VPiano::readMidiControllerSettings()
{
    SettingsFactory settings;
    for (int chan=0; chan<MIDICHANNELS; ++chan) {
        QString group = QSTR_INSTRUMENT + QString::number(chan);
        settings->beginGroup(group);
        m_lastBank[chan] = settings->value(QSTR_BANK, -1).toInt();
        m_lastProg[chan] = settings->value(QSTR_PROGRAM, 0).toInt();
        m_lastCtl[chan] = settings->value(QSTR_CONTROLLER, 1).toInt();
        settings->endGroup();

        group = QSTR_CONTROLLERS + QString::number(chan);
        settings->beginGroup(group);
        foreach(const QString& key, settings->allKeys()) {
            int ctl = key.toInt();
            int val = settings->value(key, 0).toInt();
            m_ctlSettings[chan][ctl] = val;
        }
        settings->endGroup();
    }

    settings->beginGroup(QSTR_EXTRACONTROLLERS);
    m_extraControls.clear();
    QStringList keys = settings->allKeys();
    keys.sort();
    foreach(const QString& key, keys) {
        m_extraControls << settings->value(key, QString()).toString();
    }
    settings->endGroup();
}

void VPiano::writeSettings()
{
    VPianoSettings::instance()->setGeometry(saveGeometry());
    VPianoSettings::instance()->setState(saveState());
    VPianoSettings::instance()->SaveSettings();

    SettingsFactory settings;

    for (int chan=0; chan<MIDICHANNELS; ++chan) {
        QString group = QSTR_CONTROLLERS + QString::number(chan);
        settings->beginGroup(group);
        settings->remove("");
        QMap<int,int>::const_iterator it, end;
        it = m_ctlState[chan].constBegin();
        end = m_ctlState[chan].constEnd();
        for (; it != end; ++it)
            settings->setValue(QString::number(it.key()), it.value());
        settings->endGroup();

        group = QSTR_INSTRUMENT + QString::number(chan);
        settings->beginGroup(group);
        settings->setValue(QSTR_BANK, m_lastBank[chan]);
        settings->setValue(QSTR_PROGRAM, m_lastProg[chan]);
        settings->setValue(QSTR_CONTROLLER, m_lastCtl[chan]);
        settings->endGroup();
    }

    settings->beginGroup(QSTR_EXTRACONTROLLERS);
    settings->remove("");
    int i = 0;
    foreach(const QString& ctl, m_extraControls)  {
        QString key = QString("%1").arg(i++, 2, 10, QChar('0'));
        settings->setValue(key, ctl);
    }
    settings->endGroup();

    settings->beginGroup(QSTR_SHORTCUTS);
    settings->remove("");
    QList<QAction *> actions = findChildren<QAction *> ();
    foreach(QAction *pAction, actions)
    {
        if (pAction->objectName().isEmpty())
            continue;
        const QString& sKey = '/' + pAction->objectName();
        const QString& sValue = pAction->shortcut().toString();
        QList<QKeySequence> defShortcuts = m_defaultShortcuts.value(sKey);
        if (sValue.isEmpty() && defShortcuts.count() == 0)
        {
            if (settings->contains(sKey))
                settings->remove(sKey);
        }
        else
        {
            settings->setValue(sKey, sValue);
        }
    }
    settings->endGroup();
    settings->sync();
}

void VPiano::closeEvent( QCloseEvent *event )
{
    //qDebug() << "closeEvent:" << event->type();
    if (m_initialized)
        writeSettings();
    event->accept();
}

int VPiano::getDegree(const int note) const
{
    return note % 12;
}

int VPiano::getType(const int note) const
{
    int g = getDegree(note);
    if (g == 1 || g == 3 || g == 6 || g == 8 || g == 10)
        return 1;
    return 0;
}

QColor VPiano::getColorFromPolicy(const int chan, const int note, const int vel)
{
    Q_UNUSED(vel)
    PianoPalette palette = VPianoSettings::instance()->currentPalette();
    switch (palette.paletteId()) {
    case PAL_SINGLE:
        return palette.getColor(0);
    case PAL_DOUBLE:
        return palette.getColor(getType(note));
    case PAL_CHANNELS:
        return palette.getColor(chan);
    case PAL_SCALE:
        return palette.getColor(getDegree(note));
    }
    return QColor();
}

void VPiano::slotNoteOn(const int chan, const int note, const int vel)
{
    if (VPianoSettings::instance()->channel() == chan || VPianoSettings::instance()->omniMode()) {
        if (vel == 0) {
            slotNoteOff(chan, note, vel);
        } else {
            QColor c = getColorFromPolicy(chan, note, vel);
            int v = (VPianoSettings::instance()->velocityColor() ? vel : MIDIVELOCITY );
            ui.pianokeybd->showNoteOn(note, c, v);
#ifdef ENABLE_DBUS
            emit event_noteon(note);
#endif
        }
    }
}

void VPiano::slotNoteOff(const int chan, const int note, const int vel)
{
    Q_UNUSED(vel)
    if (VPianoSettings::instance()->channel() == chan || VPianoSettings::instance()->omniMode()) {
        ui.pianokeybd->showNoteOff(note);
#ifdef ENABLE_DBUS
        emit event_noteoff(note);
#endif
    }
}

void VPiano::slotKeyPressure(const int chan, const int note, const int value)
{
#ifdef ENABLE_DBUS
    int channel = VPianoSettings::instance()->channel();
    if (channel == chan) {
        emit event_polykeypress(note, value);
    }
#else
    Q_UNUSED(chan)
    Q_UNUSED(note)
    Q_UNUSED(value)
#endif
}

void VPiano::slotController(const int chan, const int control, const int value)
{
    int channel = VPianoSettings::instance()->channel();
    if (channel == chan) {
        switch (control) {
        case CTL_ALL_SOUND_OFF:
        case CTL_ALL_NOTES_OFF:
            ui.pianokeybd->allKeysOff();
            break;
        case CTL_RESET_ALL_CTL:
            initializeAllControllers();
            break;
        default:
            updateController(control, value);
            updateExtraController(control, value);
        }
#ifdef ENABLE_DBUS
        emit event_controlchange(control, value);
#endif
    }
}

void VPiano::slotProgram(const int chan, const int program)
{
    int channel = VPianoSettings::instance()->channel();
    if (channel == chan) {
        updateProgramChange(program);
#ifdef ENABLE_DBUS
        emit event_programchange(program);
#endif
    }
}

void VPiano::slotChannelPressure(const int chan, const int value)
{
    int channel = VPianoSettings::instance()->channel();
    if (channel == chan) {
#ifdef ENABLE_DBUS
        emit event_chankeypress(value);
#else
        Q_UNUSED(value)
#endif
    }
}

void VPiano::slotPitchBend(const int chan, const int value)
{
    int channel = VPianoSettings::instance()->channel();
    if (channel == chan) {
        m_bender->setValue(value);
        m_bender->setToolTip(QString::number(value));
#ifdef ENABLE_DBUS
        emit event_pitchwheel(value);
#endif
    }
}

void VPiano::showEvent ( QShowEvent *event )
{
    //qDebug() << "showEvent:" << event->type();
    QMainWindow::showEvent(event);
#if !defined(SMALL_SCREEN)
    if (m_initialized) {
        ui.pianokeybd->setFocus();
        grabKb();
    }
#endif
}

void VPiano::hideEvent( QHideEvent *event )
{
    //qDebug() << "hideEvent:" << event->type();
//#if !defined(SMALL_SCREEN)
    //releaseKb();
//#endif
    QMainWindow::hideEvent(event);
}

void VPiano::sendNoteOn(const int midiNote, const int vel)
{
    if ((midiNote & MASK_SAFETY) == midiNote) {
        int channel = VPianoSettings::instance()->channel();
        m_midiout->sendNoteOn( channel, midiNote, vel );
    }
}

void VPiano::noteOn(const int midiNote, const int vel)
{
    sendNoteOn(midiNote, vel);
#ifdef ENABLE_DBUS
    emit event_noteon(midiNote);
#endif
}

void VPiano::sendNoteOff(const int midiNote, const int vel)
{
    if ((midiNote & MASK_SAFETY) == midiNote) {
        int channel = VPianoSettings::instance()->channel();
        m_midiout->sendNoteOff( channel, midiNote, vel );
    }
}

void VPiano::noteOff(const int midiNote, const int vel)
{
    sendNoteOff(midiNote, vel);
#ifdef ENABLE_DBUS
    emit event_noteoff(midiNote);
#endif
}

void VPiano::sendController(const int controller, const int value)
{
    int channel = VPianoSettings::instance()->channel();
    m_midiout->sendController( channel, controller, value );
}

void VPiano::resetAllControllers()
{
    sendController(CTL_RESET_ALL_CTL, 0);
    initializeAllControllers();
}

void VPiano::initializeAllControllers()
{
    int channel = VPianoSettings::instance()->channel();
    int index = m_comboControl->currentIndex();
    int ctl = m_comboControl->itemData(index).toInt();
    int val = m_ctlState[channel][ctl];
    initControllers(channel);
    m_comboControl->setCurrentIndex(index);
    m_Control->setValue(val);
    m_Control->setToolTip(QString::number(val));
    // extra controllers
    QList<QWidget *> allWidgets = ui.toolBarExtra->findChildren<QWidget *>();
    foreach(QWidget *w, allWidgets) {
        QVariant c = w->property(MIDICTLNUMBER);
        if (c.isValid()) {
            ctl = c.toInt();
            if (m_ctlState[channel].contains(ctl)) {
                val = m_ctlState[channel][ctl];
                QVariant p = w->property("value");
                if (p.isValid()) {
                    w->setProperty("value", val);
                    w->setToolTip(QString::number(val));
                    continue;
                }
                p = w->property("checked");
                if (p.isValid()) {
                    QVariant on = w->property(MIDICTLONVALUE);
                    w->setProperty("checked", (val >= on.toInt()));
                }
            }
        }
    }
}

void VPiano::allNotesOff()
{
    sendController(CTL_ALL_NOTES_OFF, 0);
    ui.pianokeybd->allKeysOff();
}

void VPiano::sendProgramChange(const int program)
{
    int channel = VPianoSettings::instance()->channel();
    m_midiout->sendProgram( channel, program );
}

void VPiano::sendBankChange(const int bank)
{
    int channel = VPianoSettings::instance()->channel();
    int method = (m_ins != nullptr) ? m_ins->bankSelMethod() : 0;
    int lsb, msb;
    switch (method) {
    case 0:
        lsb = CALC_LSB(bank);
        msb = CALC_MSB(bank);
        sendController(CTL_MSB, msb);
        sendController(CTL_LSB, lsb);
        break;
    case 1:
        sendController(CTL_MSB, bank);
        break;
    case 2:
        sendController(CTL_LSB, bank);
        break;
    default: /* if method is 3 or above, do nothing */
        break;
    }
    m_lastBank[channel] = bank;
}

void VPiano::sendPolyKeyPress(const int note, const int value)
{
    int channel = VPianoSettings::instance()->channel();
    m_midiout->sendKeyPressure( channel, note, value );
}

void VPiano::sendChanKeyPress(const int value)
{
    int channel = VPianoSettings::instance()->channel();
    m_midiout->sendChannelPressure( channel, value );
}

void VPiano::sendBender(const int value)
{
    int channel = VPianoSettings::instance()->channel();
    m_midiout->sendPitchBend( channel, value );
}

void VPiano::slotPanic()
{
    allNotesOff();
}

void VPiano::slotResetAllControllers()
{
    resetAllControllers();
}

void VPiano::slotResetBender()
{
    m_bender->setValue(0);
    sendBender(0);
}

void VPiano::sendSysex(const QByteArray& data)
{
    m_midiout->sendSysex( data );
}

void VPiano::slotControlClicked(const bool boolValue)
{
    QObject *s = sender();
    QVariant p = s->property(MIDICTLNUMBER);
    if (p.isValid()) {
        int controller = p.toInt();
        if (controller < 128) {
            QVariant on = s->property(MIDICTLONVALUE);
            QVariant off = s->property(MIDICTLOFFVALUE);
            int value = boolValue ? on.toInt() : off.toInt();
            sendController( controller, value );
            updateController( controller, value );
        } else {
            QVariant data = s->property(SYSEXFILEDATA);
            sendSysex(data.toByteArray());
        }
    }
}

void VPiano::slotVelocityValueChanged(int value)
{
    VPianoSettings::instance()->setVelocity(value);
    setWidgetTip(m_Velocity, value);
    ui.pianokeybd->setVelocity(value);
}

void VPiano::slotExtraController(const int value)
{
    QWidget *w = static_cast<QWidget *>(sender());
    QVariant p = w->property(MIDICTLNUMBER);
    if (p.isValid()) {
        int controller = p.toInt();
        sendController( controller, value );
        updateController( controller, value );
        setWidgetTip(w, value);
    }
}

void VPiano::slotControlSliderMoved(const int value)
{
    int index = m_comboControl->currentIndex();
    int controller = m_comboControl->itemData(index).toInt();
    int channel = VPianoSettings::instance()->channel();
    sendController( controller, value );
    updateExtraController( controller, value );
    m_ctlState[channel][controller] = value;
    setWidgetTip(m_Control, value);
}

void VPiano::slotBenderSliderMoved(const int pos)
{
    sendBender(pos);
    setWidgetTip(m_bender, pos);
}

void VPiano::slotBenderSliderReleased()
{
    m_bender->setValue(0);
    sendBender(0);
    setWidgetTip(m_bender, 0);
}

void VPiano::slotAbout()
{
    releaseKb();
    QPointer<About> dlgAbout = new About(this);
    dlgAbout->exec();
    grabKb();
    delete dlgAbout;
}

void VPiano::slotAboutQt()
{
    releaseKb();
    QApplication::aboutQt();
    grabKb();
}

void VPiano::slotConnections()
{
    QPointer<MidiSetup> dlgMidiSetup = new MidiSetup(this);
    dlgMidiSetup->setInputs(m_backendManager->availableInputs());
    dlgMidiSetup->setOutputs(m_backendManager->availableOutputs());
    dlgMidiSetup->setInput(m_midiin);
    dlgMidiSetup->setOutput(m_midiout);
    dlgMidiSetup->refresh();
    releaseKb();
    if (dlgMidiSetup->exec() == QDialog::Accepted) {
        if (m_midiin != nullptr) {
            VPianoSettings::instance()->setLastInputConnection(m_midiin->currentConnection().first);
            m_midiin->disconnect();
        }
        if (m_midiout != nullptr) {
            VPianoSettings::instance()->setLastOutputConnection(m_midiout->currentConnection().first);
            m_midiout->disconnect();
        }

        m_midiin = dlgMidiSetup->getInput();
        m_midiout = dlgMidiSetup->getOutput();

        if (m_midiin != nullptr) {
            connect(m_midiin, SIGNAL(midiNoteOn(int,int,int)), SLOT(slotNoteOn(int,int,int)), Qt::QueuedConnection);
            connect(m_midiin, SIGNAL(midiNoteOff(int,int,int)), SLOT(slotNoteOff(int,int,int)), Qt::QueuedConnection);
            connect(m_midiin, SIGNAL(midiKeyPressure(int,int,int)), SLOT(slotKeyPressure(int,int,int)), Qt::QueuedConnection);
            connect(m_midiin, SIGNAL(midiChannelPressure(int,int)), SLOT(slotChannelPressure(int,int)), Qt::QueuedConnection);
            connect(m_midiin, SIGNAL(midiController(int,int,int)), SLOT(slotController(int,int,int)), Qt::QueuedConnection);
            connect(m_midiin, SIGNAL(midiProgram(int,int)), SLOT(slotProgram(int,int)), Qt::QueuedConnection);
            connect(m_midiin, SIGNAL(midiPitchBend(int,int)), SLOT(slotPitchBend(int,int)), Qt::QueuedConnection);
        }
        enforceMIDIChannelState();
    }
    grabKb();
    delete dlgMidiSetup;
}

void VPiano::initControllers(int channel)
{
    if (m_ins != nullptr) {
        InstrumentData controls = m_ins->control();
        InstrumentData::ConstIterator it, end;
        it = controls.constBegin();
        end = controls.constEnd();
        for( ; it != end; ++it ) {
            int ctl = it.key();
            switch (ctl) {
            case CTL_VOLUME:
                m_ctlState[channel][CTL_VOLUME] = MIDIVOLUME;
                break;
            case CTL_PAN:
                m_ctlState[channel][CTL_PAN] = MIDIPAN;
                break;
            case CTL_EXPRESSION:
                m_ctlState[channel][CTL_EXPRESSION] = MIDIMAXVALUE;
                break;
            case CTL_REVERB_SEND:
                m_ctlState[channel][CTL_REVERB_SEND] = MIDIMAXVALUE;
                break;
            default:
                m_ctlState[channel][ctl] = 0;
            }
        }
    }
}

void VPiano::populateControllers()
{
    m_comboControl->blockSignals(true);
    m_comboControl->clear();
    if (m_ins != nullptr) {
        InstrumentData controls = m_ins->control();
        InstrumentData::ConstIterator it, end = controls.constEnd();
        for( it = controls.constBegin(); it != end; ++it )
            m_comboControl->addItem(it.value(), it.key());
    }
    m_comboControl->blockSignals(false);
}

void VPiano::applyPreferences()
{
    ui.pianokeybd->allKeysOff();
    ui.pianokeybd->setFont(VPianoSettings::instance()->namesFont());
    ui.pianokeybd->setLabelOctave(VPianoSettings::instance()->namesOctave());
    if ( ui.pianokeybd->numKeys() != VPianoSettings::instance()->numKeys() ||
         ui.pianokeybd->startKey() != VPianoSettings::instance()->startingKey() )
    {
        ui.pianokeybd->setNumKeys(VPianoSettings::instance()->numKeys(), VPianoSettings::instance()->startingKey());
    }
#if defined(RAWKBD_SUPPORT)
    m_filter->setRawKbdEnabled(VPianoSettings::instance()->rawKeyboard());
#endif
    ui.pianokeybd->setRawKeyboardMode(VPianoSettings::instance()->rawKeyboard());
    ui.pianokeybd->setVelocityTint(VPianoSettings::instance()->velocityColor());
    ui.pianokeybd->setVelocity(VPianoSettings::instance()->velocity());
    bool enableKeyboard = VPianoSettings::instance()->enableKeyboard();
    bool enableMouse = VPianoSettings::instance()->enableMouse();
    bool enableTouch = VPianoSettings::instance()->enableTouch();
    ui.pianokeybd->setKeyboardEnabled(enableKeyboard);
    ui.pianokeybd->setMouseEnabled(enableMouse);
    ui.pianokeybd->setTouchEnabled(enableTouch);
    ui.actionKeyboardInput->setChecked(enableKeyboard);
    ui.actionMouseInput->setChecked(enableMouse);
    ui.actionTouchScreenInput->setChecked(enableTouch);

    VMPKKeyboardMap* map = VPianoSettings::instance()->getKeyboardMap();
    if (!map->getFileName().isEmpty() && map->getFileName() != QSTR_DEFAULT )
        ui.pianokeybd->setKeyboardMap(map);
    else
        ui.pianokeybd->resetKeyboardMap();

    map = VPianoSettings::instance()->getRawKeyboardMap();
    if (!map->getFileName().isEmpty() && map->getFileName() != QSTR_DEFAULT )
        ui.pianokeybd->setRawKeyboardMap(map);
    else
        ui.pianokeybd->resetRawKeyboardMap();

    ui.pianokeybd->setHighlightPalette(VPianoSettings::instance()->currentPalette());
    ui.pianokeybd->setBackgroundPalette(VPianoSettings::instance()->getPalette(PAL_SCALE));
    ui.pianokeybd->setShowColorScale(ui.actionColorScale->isChecked());

    populateInstruments();
    populateControllers();

    QPoint wpos = pos();
    Qt::WindowFlags flags = windowFlags();
    if (VPianoSettings::instance()->alwaysOnTop())
        flags |= Qt::WindowStaysOnTopHint;
    else
        flags &= ~Qt::WindowStaysOnTopHint;
    setWindowFlags( flags );
    move(wpos);

    ui.pianokeybd->setShowLabels(VPianoSettings::instance()->namesVisibility());
}

void VPiano::populateInstruments()
{
    m_ins = nullptr;
    m_comboBank->clear();
    m_comboProg->clear();
    int channel = VPianoSettings::instance()->channel();
    //qDebug() << Q_FUNC_INFO << VPianoSettings::instance()->insFileName();
    if (!VPianoSettings::instance()->insFileName().isEmpty() &&
         VPianoSettings::instance()->insFileName() != QSTR_DEFAULT) {
        if (channel == VPianoSettings::instance()->drumsChannel())
            m_ins = VPianoSettings::instance()->getDrumsInstrument();
        else
            m_ins = VPianoSettings::instance()->getInstrument();
        if (m_ins != nullptr) {
            //qDebug() << "Instrument Name:" << m_ins->instrumentName();
            //qDebug() << "Bank Selection method: " << m_ins->bankSelMethod();
            InstrumentPatches patches = m_ins->patches();
            InstrumentPatches::ConstIterator j;
            for( j = patches.constBegin(); j != patches.constEnd(); ++j ) {
                //if (j.key() < 0) continue;
                InstrumentData patch = j.value();
                m_comboBank->addItem(patch.name(), j.key());
                //qDebug() << "---- Bank[" << j.key() << "]=" << patch.name();
            }
            updateBankChange(m_lastBank[channel]);
        }
    }
}

void VPiano::applyInitialSettings()
{
    int idx, ctl, channel;
    for ( int ch=0; ch<MIDICHANNELS; ++ch) {
        initControllers(ch);
        QMap<int,int>::Iterator i, j, end;
        i = m_ctlSettings[ch].begin();
        end = m_ctlSettings[ch].end();
        for (; i != end; ++i) {
            j = m_ctlState[ch].find(i.key());
            if (j != m_ctlState[ch].end())
                m_ctlState[ch][i.key()] = i.value();
        }
    }
    channel = VPianoSettings::instance()->channel();
    ctl = m_lastCtl[channel];
    idx = m_comboControl->findData(ctl);
    if (idx != -1)
        m_comboControl->setCurrentIndex(idx);
    updateBankChange(m_lastBank[channel]);
    idx = m_comboProg->findData(m_lastProg[channel]);
    m_comboProg->setCurrentIndex(idx);
}

void VPiano::slotPreferences()
{
    QPointer<Preferences> dlgPreferences = new Preferences(this);
    dlgPreferences->setNoteNames(ui.pianokeybd->noteNames());
    releaseKb();
    if (dlgPreferences->exec() == QDialog::Accepted) {
        applyPreferences();
    }
    delete dlgPreferences;
    grabKb();
}

QString VPiano::dataDirectory()
{
#if defined(Q_OS_WIN32)
    return QApplication::applicationDirPath() + "/";
#elif defined(Q_OS_MAC)
    return QApplication::applicationDirPath() + "/../Resources/";
#elif defined(Q_OS_UNIX)
    return QApplication::applicationDirPath() + "/../share/vmpk/";
#else
    return QString();
#endif
}

QString VPiano::localeDirectory()
{
#if defined(Q_OS_LINUX)
    return VPiano::dataDirectory() + "locale/";
#elif defined(Q_OS_WIN)
    return VPiano::dataDirectory() + "translations/";
#else
    return VPiano::dataDirectory();
#endif
}

void VPiano::slotEditKeyboardMap()
{
#if !defined(SMALL_SCREEN)
    VMPKKeyboardMap* map = nullptr;
    if (VPianoSettings::instance()->rawKeyboard()) {
        map = VPianoSettings::instance()->getRawKeyboardMap();
        map->copyFrom(ui.pianokeybd->getRawKeyboardMap(), true);
    } else {
        map = VPianoSettings::instance()->getKeyboardMap();
        map->copyFrom(ui.pianokeybd->getKeyboardMap(), false);
    }
    QPointer<KMapDialog> dlgKeyMap = new KMapDialog(this);
    dlgKeyMap->displayMap(map);
    releaseKb();
    if (dlgKeyMap->exec() == QDialog::Accepted) {
        dlgKeyMap->getMap(map);
        if (VPianoSettings::instance()->rawKeyboard()) {
            ui.pianokeybd->setRawKeyboardMap(map);
        } else {
            ui.pianokeybd->setKeyboardMap(map);
        }
    }
    grabKb();
    delete dlgKeyMap;
#endif
}

void VPiano::populatePrograms(int bank)
{
    if (bank < 0)
        return;
    m_comboProg->clear();
    if (m_ins != nullptr) {
        InstrumentData patch = m_ins->patch(bank);
        InstrumentData::ConstIterator k;
        for( k = patch.constBegin(); k != patch.constEnd(); ++k )
            m_comboProg->addItem(k.value(), k.key());
            //qDebug() << "patch[" << k.key() << "]=" << k.value();
    }
}

void VPiano::slotComboBankActivated(const int index)
{
    int idx = index;
    if (idx < 0)
        m_comboBank->setCurrentIndex(idx = 0);
    int bank = m_comboBank->itemData(idx).toInt();
    populatePrograms(bank);
    slotComboProgActivated();
}

void VPiano::slotComboProgActivated(const int index)
{
    int idx = index;
    int channel = VPianoSettings::instance()->channel();
    if (idx < 0)
        m_comboProg->setCurrentIndex(idx = 0);
    int bankIdx = m_comboBank->currentIndex();
    int bank = m_comboBank->itemData(bankIdx).toInt();
    if (bank >= 0) {
        sendBankChange(bank);
        m_lastBank[channel] = bank;
    }
    int pgm = m_comboProg->itemData(idx).toInt();
    if (pgm >= 0) {
        sendProgramChange(pgm);
        m_lastProg[channel] = pgm;
    }
    updateNoteNames(channel == VPianoSettings::instance()->drumsChannel());
}

void VPiano::slotBaseOctaveValueChanged(const int octave)
{
    if (octave != VPianoSettings::instance()->baseOctave()) {
        ui.pianokeybd->allKeysOff();
        ui.pianokeybd->setBaseOctave(octave);
        VPianoSettings::instance()->setBaseOctave(octave);
    }
}

void VPiano::slotTransposeValueChanged(const int transpose)
{
    if (transpose != VPianoSettings::instance()->transpose()) {
        ui.pianokeybd->setTranspose(transpose);
        VPianoSettings::instance()->setTranspose(transpose);
    }
}

void VPiano::updateNoteNames(bool drums)
{
    if (drums && (m_ins != nullptr)) {
        int channel = VPianoSettings::instance()->drumsChannel();
        int b = m_lastBank[channel];
        int p = m_lastProg[channel];
        const InstrumentData& notes = m_ins->notes(b, p);
        QStringList noteNames;
        for(int n=0; n<128; ++n) {
            if (notes.contains(n))
                noteNames << notes[n];
            else
                noteNames << QString();
        }
        ui.pianokeybd->useCustomNoteNames(noteNames);
    } else
        ui.pianokeybd->useStandardNoteNames();
}

void VPiano::slotChannelValueChanged(const int channel)
{
    int idx;
    int c = channel - 1;
    int baseChannel = VPianoSettings::instance()->channel();
    if (c != baseChannel) {
        int drms = VPianoSettings::instance()->drumsChannel();
        bool updDrums = ((c == drms) || (baseChannel == drms));
        baseChannel = c;
        VPianoSettings::instance()->setChannel(c);
        ui.pianokeybd->setChannel(c);
        if (updDrums) {
            populateInstruments();
            populateControllers();
        }
        idx = m_comboControl->findData(m_lastCtl[baseChannel]);
        if (idx != -1) {
            int ctl = m_lastCtl[baseChannel];
            m_comboControl->setCurrentIndex(idx);
            updateController(ctl, m_ctlState[baseChannel][ctl]);
            updateExtraController(ctl, m_ctlState[baseChannel][ctl]);
        }
        updateBankChange(m_lastBank[baseChannel]);
        updateProgramChange(m_lastProg[baseChannel]);
        enforceMIDIChannelState();
        ui.pianokeybd->resetKeyPressedColor();
    }
}

void VPiano::updateController(int ctl, int val)
{
    int index = m_comboControl->currentIndex();
    int controller = m_comboControl->itemData(index).toInt();
    int channel = VPianoSettings::instance()->channel();
    if (controller == ctl) {
        m_Control->setValue(val);
        m_Control->setToolTip(QString::number(val));
    }
    m_ctlState[channel][ctl] = val;
    if ((ctl == CTL_MSB || ctl == CTL_LSB ) && m_ins != nullptr) {
        if (m_ins->bankSelMethod() == 0)
            m_lastBank[channel] = m_ctlState[channel][CTL_MSB] << 7 |
                                    m_ctlState[channel][CTL_LSB];
        else
            m_lastBank[channel] = val;

        updateBankChange(m_lastBank[channel]);
    }
}

void VPiano::updateExtraController(int ctl, int val)
{
    QList<QWidget *> allWidgets = ui.toolBarExtra->findChildren<QWidget *>();
    foreach(QWidget *w, allWidgets) {
        QVariant p = w->property(MIDICTLNUMBER);
        if (p.isValid() && p.toInt() == ctl) {
            QVariant v = w->property("value");
            if (v.isValid() && v.toInt() != val) {
                w->setProperty("value", val);
                w->setToolTip(QString::number(val));
                continue;
            }
            v = w->property("checked");
            if (v.isValid()) {
                QVariant on = w->property(MIDICTLONVALUE);
                bool checked = (val >= on.toInt());
                w->setProperty("checked", checked);
            }
        }
    }
}

void VPiano::updateBankChange(int bank)
{
    int idx;
    int channel = VPianoSettings::instance()->channel();
    if (bank < 0) {
        m_comboBank->setCurrentIndex(idx = 0);
        bank = m_comboBank->itemData(idx).toInt();
        if (bank < 0)
            bank = 0;
    } else {
        idx = m_comboBank->findData(bank);
        if (idx != -1) {
            m_comboBank->setCurrentIndex(idx);
            m_lastBank[channel] = bank;
        }
    }
    populatePrograms(bank);
    updateProgramChange();
}

void VPiano::updateProgramChange(int program)
{
    int idx;
    int channel = VPianoSettings::instance()->channel();
    if (program < 0) {
        m_comboProg->setCurrentIndex(idx = 0);
        program = m_comboProg->itemData(idx).toInt();
    } else {
        idx = m_comboProg->findData(program);
        if (idx != -1) {
            m_comboProg->setCurrentIndex(idx);
            m_lastProg[channel] = program;
        }
    }
    updateNoteNames(channel == VPianoSettings::instance()->drumsChannel());
}

void VPiano::slotComboControlCurrentIndexChanged(const int index)
{
    int channel = VPianoSettings::instance()->channel();
    int ctl = m_comboControl->itemData(index).toInt();
    int val = m_ctlState[channel][ctl];
    m_Control->setValue(val);
    m_Control->setToolTip(QString::number(val));
    m_lastCtl[channel] = ctl;
}

void VPiano::grabKb()
{
#if defined(RAWKBD_SUPPORT)
    m_filter->setRawKbdEnabled(VPianoSettings::instance()->rawKeyboard());
#endif
}

void VPiano::releaseKb()
{
#if defined(RAWKBD_SUPPORT)
    m_filter->setRawKbdEnabled(false);
#endif
}

#if defined(SMALL_SCREEN)
class HelpDialog : public QDialog
{
public:
    HelpDialog(const QUrl &document, QWidget *parent = nullptr) : QDialog(parent)
    {
        setWindowState(Qt::WindowMaximized | Qt::WindowActive);
        QVBoxLayout *layout = new QVBoxLayout(this);
        QTextBrowser *browser = new QTextBrowser(this);
        layout->addWidget(browser);
        QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);
        layout->addWidget(buttonBox);
        browser->setSource(document);
        connect(buttonBox, SIGNAL(accepted()), SLOT(close()));
    }
};
#endif

void VPiano::slotHelpContents()
{
    QStringList hlps;
    QLocale loc(configuredLanguage());
    QStringList lc = loc.name().split("_");
    hlps += QSTR_HELPL.arg(loc.name());
    if (lc.count() > 1)
        hlps += QSTR_HELPL.arg(lc[0]);
    hlps += QSTR_HELP;
    QDir hlpDir(VPiano::dataDirectory());
    foreach(const QString& hlp_name, hlps) {
        if (hlpDir.exists(hlp_name)) {
            QUrl url = QUrl::fromLocalFile(hlpDir.absoluteFilePath(hlp_name));
#if defined(SMALL_SCREEN)
            HelpDialog hlpDlg(url, this);
            hlpDlg.exec();
#else
            QDesktopServices::openUrl(url);
#endif
            return;
        }
    }
    QMessageBox::critical(this, tr("Error"), tr("No help file found"));
}

void VPiano::slotOpenWebSite()
{
    QUrl url(QSTR_VMPKURL);
    QDesktopServices::openUrl(url);
}

void VPiano::slotImportSF()
{
    QPointer<RiffImportDlg> dlgRiffImport = new RiffImportDlg(this);
    releaseKb();
    if ((dlgRiffImport->exec() == QDialog::Accepted) &&
        !dlgRiffImport->getOutput().isEmpty()) {
        dlgRiffImport->save();
        VPianoSettings::instance()->setInsFileName(dlgRiffImport->getOutput());
        VPianoSettings::instance()->setInsName(dlgRiffImport->getName());
        applyPreferences();
    }
    grabKb();
    delete dlgRiffImport;
}

void VPiano::slotEditExtraControls()
{
    QPointer<DialogExtraControls> dlgExtra = new DialogExtraControls(this);
    dlgExtra->setControls(m_extraControls);
    releaseKb();
    if (dlgExtra->exec() == QDialog::Accepted) {
        m_extraControls = dlgExtra->getControls();
        clearExtraControllers();
        initExtraControllers();
    }
    grabKb();
    delete dlgExtra;
}

void VPiano::setWidgetTip(QWidget* w, int val)
{
    QString tip = QString::number(val);
    w->setToolTip(tip);
    QToolTip::showText(w->parentWidget()->mapToGlobal(w->pos()), tip);
}

//void VPiano::slotEditPrograms()
//{ }

#if defined(ENABLE_DBUS)
void VPiano::quit()
{
    close();
}

void VPiano::panic()
{
    allNotesOff();
}

void VPiano::reset_controllers()
{
    resetAllControllers();
}

void VPiano::channel(int value)
{
    if (value >= 0 && value < MIDICHANNELS)
        m_sboxChannel->setValue(value + 1);
}

void VPiano::octave(int value)
{
    m_sboxOctave->setValue(value);
}

void VPiano::transpose(int value)
{
    m_sboxTranspose->setValue(value);
}

void VPiano::velocity(int value)
{
    m_Velocity->setValue(value);
}

void VPiano::connect_in(const QString &value)
{
    if( m_midiin != nullptr) {
        m_midiin->close();
        if (!value.isEmpty()) {
            for(const MIDIConnection& conn : m_midiin->connections()) {
                if (conn.first == value) {
                    m_midiin->open(conn);
                    break;
                }
            }
        }
    }
}

void VPiano::connect_out(const QString &value)
{
    if( m_midiout != nullptr) {
        m_midiout->close();
        if (!value.isEmpty()) {
            for(const MIDIConnection& conn : m_midiout->connections()) {
                if (conn.first == value) {
                    m_midiout->open(conn);
                    if (m_midiin != nullptr) {
                        m_midiin->setMIDIThruDevice(m_midiout);
                        m_midiin->enableMIDIThru(VPianoSettings::instance()->midiThru());
                    }
                    break;
                }
            }
        }
    }
}

void VPiano::connect_thru(bool value)
{
    if( m_midiin != 0 && m_midiout != 0) {
        m_midiin->enableMIDIThru(value);
        m_midiin->setMIDIThruDevice(m_midiout);
    }
}

void VPiano::noteoff(int note)
{
    sendNoteOff(note, 0);
}

void VPiano::noteon(int note)
{
    sendNoteOn(note, VPianoSettings::instance()->velocity());
}

void VPiano::polykeypress(int note, int value)
{
    sendPolyKeyPress(note, value);
}

void VPiano::controlchange(int control, int value)
{
    sendController(control, value);
}

void VPiano::programchange(int value)
{
    sendProgramChange(value);
}

void VPiano::programnamechange(const QString &value)
{
    int idx = m_comboProg->findText(value, Qt::MatchFixedString);
    if (idx != -1) {
        int prg = m_comboProg->itemData(idx).toInt();
        programchange(prg);
    }
}

void VPiano::chankeypress(int value)
{
    sendChanKeyPress(value);
}

void VPiano::pitchwheel(int value)
{
    sendBender(value);
}

#endif /* ENABLE_DBUS */

void VPiano::slotShortcuts()
{
#if !defined(SMALL_SCREEN)
    QPointer<ShortcutDialog> shcutDlg = new ShortcutDialog(findChildren<QAction*>(), this);
    releaseKb();
    shcutDlg->setDefaultShortcuts(m_defaultShortcuts);
    shcutDlg->exec();
    grabKb();
    delete shcutDlg;
#endif
}

void VPiano::slotBankNext()
{
    int index = m_comboBank->currentIndex();
    if (index < m_comboBank->count()-1) {
        m_comboBank->setCurrentIndex(++index);
        slotComboBankActivated(index);
    }
}

void VPiano::slotBankPrev()
{
    int index = m_comboBank->currentIndex();
    if (index > 0) {
        m_comboBank->setCurrentIndex(--index);
        slotComboBankActivated(index);
    }
}

void VPiano::slotProgramNext()
{
    int index = m_comboProg->currentIndex();
    if (index < m_comboProg->count()-1) {
        m_comboProg->setCurrentIndex(++index);
        slotComboProgActivated(index);
    }
}

void VPiano::slotProgramPrev()
{
    int index = m_comboProg->currentIndex();
    if (index > 0) {
        m_comboProg->setCurrentIndex(--index);
        slotComboProgActivated(index);
    }
}

void VPiano::slotControllerNext()
{
    int index = m_comboControl->currentIndex();
    if (index < m_comboControl->count()-1)
        m_comboControl->setCurrentIndex(++index);
}

void VPiano::slotControllerPrev()
{
    int index = m_comboControl->currentIndex();
    if (index > 0)
        m_comboControl->setCurrentIndex(--index);
}

void VPiano::slotVelocityUp()
{
    m_Velocity->triggerAction(QDial::SliderPageStepAdd);
}

void VPiano::slotVelocityDown()
{
    m_Velocity->triggerAction(QDial::SliderPageStepSub);
}

void VPiano::slotControllerUp()
{
    m_Control->triggerAction(QDial::SliderPageStepAdd);
    slotControlSliderMoved(m_Control->value());
}

void VPiano::slotControllerDown()
{
    m_Control->triggerAction(QDial::SliderPageStepSub);
    slotControlSliderMoved(m_Control->value());
}

QString VPiano::configuredLanguage()
{
    QString language = VPianoSettings::instance()->language();
    if (language.isEmpty()) {
        SettingsFactory settings;
        QString defLang = QLocale::system().name();
        settings->beginGroup(QSTR_PREFERENCES);
        language = settings->value(QSTR_LANGUAGE, defLang).toString();
        settings->endGroup();
    }
    //qDebug() << Q_FUNC_INFO << language;
    return language;
}

void VPiano::slotSwitchLanguage(QAction *action)
{
    QString lang = action->data().toString();
    if ( QMessageBox::question (this, tr("Language Changed"),
            tr("The language for this application is going to change to %1. "
               "Do you want to continue?").arg(m_supportedLangs[lang]),
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
    {
        VPianoSettings::instance()->setLanguage(lang);
        retranslateUi();
    } else {
        m_currentLang->setChecked(true);
    }
}

void VPiano::createLanguageMenu()
{
    QString currentLang = configuredLanguage();
    QActionGroup *languageGroup = new QActionGroup(this);
    connect(languageGroup, SIGNAL(triggered(QAction *)),
            SLOT(slotSwitchLanguage(QAction *)));
    QDir dir(VPiano::localeDirectory());
    QStringList fileNames = dir.entryList(QStringList(QSTR_VMPKPX + "*.qm"));
    QStringList locales;
    locales << "en";
    foreach (const QString& fileName, fileNames) {
        QString locale = fileName;
        locale.remove(0, locale.indexOf('_') + 1);
        locale.truncate(locale.lastIndexOf('.'));
        locales << locale;
    }
    locales.sort();
    foreach (const QString& loc, locales) {
        QAction *action = new QAction(m_supportedLangs.value(loc), this);
        action->setCheckable(true);
        action->setData(loc);
        ui.menuLanguage->addAction(action);
        languageGroup->addAction(action);
        if (currentLang.startsWith(loc)) {
            action->setChecked(true);
            m_currentLang = action;
        }
    }
}

void VPiano::slotAboutTranslation()
{
    QString common = tr("<p>VMPK is developed and translated thanks to the "
        "volunteer work of many people from around the world. If you want to "
        "join the team or have any question, please visit the forums at "
        "<a href='http://sourceforge.net/projects/vmpk/forums'>SourceForge</a>"
        "</p>");
    QString currentLang = configuredLanguage();
    bool supported(false);
    if (!currentLang.startsWith("en")) {
        QMapIterator<QString,QString> it(m_supportedLangs);
        while (it.hasNext())  {
            it.next();
            if (currentLang.startsWith(it.key())) {
                supported = true;
                break;
            }
        }
    }
    if (supported)
        QMessageBox::information(this, tr("Translation"),
            tr("<p>Translation by TRANSLATOR_NAME_AND_EMAIL</p>%1").arg(common));
    else
        QMessageBox::information(this, tr("Translation Information"), common);
}

void VPiano::retranslateUi()
{
    m_trq->load( QSTR_QTPX + configuredLanguage(),
                 QLibraryInfo::location(QLibraryInfo::TranslationsPath) );
    m_trp->load( QSTR_VMPKPX + configuredLanguage(),
                 VPiano::localeDirectory() );
    ui.retranslateUi(this);
    ui.pianokeybd->retranslate();
    initLanguages();
    ui.menuLanguage->clear();
    createLanguageMenu();
    retranslateToolbars();
}

void VPiano::initLanguages()
{
    m_supportedLangs.clear();
    m_supportedLangs.insert(QLatin1String("cs"), tr("Czech"));
    m_supportedLangs.insert(QLatin1String("de"), tr("German"));
    m_supportedLangs.insert(QLatin1String("en"), tr("English"));
    m_supportedLangs.insert(QLatin1String("es"), tr("Spanish"));
    m_supportedLangs.insert(QLatin1String("fr"), tr("French"));
    m_supportedLangs.insert(QLatin1String("gl"), tr("Galician"));
    m_supportedLangs.insert(QLatin1String("nl"), tr("Dutch"));
    m_supportedLangs.insert(QLatin1String("ru"), tr("Russian"));
    m_supportedLangs.insert(QLatin1String("sr"), tr("Serbian"));
    m_supportedLangs.insert(QLatin1String("sv"), tr("Swedish"));
    m_supportedLangs.insert(QLatin1String("zh_CN"), tr("Chinese"));
}

void VPiano::retranslateToolbars()
{
    m_lblChannel->setText(
#if defined(SMALL_SCREEN)
        tr("Chan:")
#else
        tr("Channel:")
#endif
    );
    m_lblBaseOctave->setText(
#if defined(SMALL_SCREEN)
        tr("Oct:")
#else
        tr("Base Octave:")
#endif
    );
    m_lblTranspose->setText(
#if defined(SMALL_SCREEN)
        tr("Trans:")
#else
        tr("Transpose:")
#endif
    );
    m_lblVelocity->setText(
#if defined(SMALL_SCREEN)
        tr("Vel:")
#else
        tr("Velocity:")
#endif
    );
    m_lblBank->setText(tr("Bank:"));
    m_lblBender->setText(tr("Bender:"));
    m_lblControl->setText(tr("Control:"));
    m_lblProgram->setText(tr("Program:"));
    m_lblValue->setText(tr("Value:"));
}

QMenu * VPiano::createPopupMenu ()
{
#if defined(SMALL_SCREEN)
    return 0;
#else
    return QMainWindow::createPopupMenu();
#endif
}

void VPiano::enforceMIDIChannelState()
{
    if (VPianoSettings::instance()->enforceChannelState()) {
        int channel = VPianoSettings::instance()->channel();
        //qDebug() << Q_FUNC_INFO << "channel=" << m_channel << endl;
        QMap<int,int>::Iterator i, end;
        i = m_ctlSettings[channel].begin();
        end = m_ctlSettings[channel].end();
        for (; i != end; ++i) {
            //qDebug() << "ctl=" << i.key() << "val=" << i.value();
            sendController(i.key(), i.value());
        }
        //qDebug() << "bank=" << m_lastBank[m_channel];
        sendBankChange(m_lastBank[channel]);
        //qDebug() << "prog=" << m_lastProg[m_channel];
        sendProgramChange(m_lastProg[channel]);
    }
}

void VPiano::slotKeyboardInput(bool value)
{
    VPianoSettings::instance()->setEnableKeyboard(value);
    ui.pianokeybd->setKeyboardEnabled(value);
}

void VPiano::slotMouseInput(bool value)
{
    VPianoSettings::instance()->setEnableMouse(value);
    ui.pianokeybd->setMouseEnabled(value);
}

void VPiano::slotTouchScreenInput(bool value)
{
    VPianoSettings::instance()->setEnableTouch(value);
    ui.pianokeybd->setTouchEnabled(value);
}

void VPiano::slotColorPolicy()
{
    QPointer<ColorDialog> dlgColorPolicy = new ColorDialog(this);
    dlgColorPolicy->loadPalette(VPianoSettings::instance()->currentPalette().paletteId());
    if (dlgColorPolicy->exec() == QDialog::Accepted) {
        int pal = dlgColorPolicy->selectedPalette();
        PianoPalette editedPalette = VPianoSettings::instance()->getPalette(pal);
        if (pal >= PAL_SINGLE && pal < PAL_SCALE) {
            VPianoSettings::instance()->setCurrentPalette(pal);
            ui.pianokeybd->setHighlightPalette(editedPalette);
        } else if (pal == PAL_SCALE) {
            ui.pianokeybd->setBackgroundPalette(editedPalette);
        }
    }
    delete dlgColorPolicy;
}

void VPiano::slotColorScale(bool value)
{
    VPianoSettings::instance()->setColorScale(value);
    ui.pianokeybd->setShowColorScale(value);
}

void VPiano::toggleWindowFrame(const bool state)
{
    Qt::WindowFlags flags = windowFlags();

    if (state)
        flags &= ~Qt::FramelessWindowHint;
    else
        flags |= Qt::FramelessWindowHint;

    setWindowFlags(flags);
    show();
}

void VPiano::setPortableConfig(const QString fileName)
{
    if (fileName.isEmpty()) {
        QFileInfo appInfo(QCoreApplication::applicationFilePath());
#if defined(Q_OS_MACOS)
        CFURLRef url = static_cast<CFURLRef>(CFAutorelease(static_cast<CFURLRef>(CFBundleCopyBundleURL(CFBundleGetMainBundle()))));
        QString path = QUrl::fromCFURL(url).path() + "../";
        QFileInfo cfgInfo(path, appInfo.baseName() + ".conf");
#else
        QFileInfo cfgInfo(appInfo.absoluteDir(), appInfo.baseName() + ".conf");
#endif
        drumstick::widgets::SettingsFactory::setFileName(cfgInfo.absoluteFilePath());
    } else {
        drumstick::widgets::SettingsFactory::setFileName(fileName);
    }
}

void VPiano::slotNameOrientation(QAction* action)
{
    if(action == ui.actionHorizontal) {
        VPianoSettings::instance()->setNamesOrientation(PianoKeybd::HorizontalOrientation);
    } else if(action == ui.actionVertical) {
        VPianoSettings::instance()->setNamesOrientation(PianoKeybd::VerticalOrientation);
    } else if(action == ui.actionAutomatic) {
        VPianoSettings::instance()->setNamesOrientation(PianoKeybd::AutomaticOrientation);
    }
    ui.pianokeybd->setLabelOrientation(VPianoSettings::instance()->namesOrientation());
}

void VPiano::slotNameVisibility(QAction* action)
{
    if(action == ui.actionNever) {
        VPianoSettings::instance()->setNamesVisibility(PianoKeybd::ShowNever);
    } else if(action == ui.actionMinimal) {
        VPianoSettings::instance()->setNamesVisibility(PianoKeybd::ShowMinimum);
    } else if(action == ui.actionWhen_Activated) {
        VPianoSettings::instance()->setNamesVisibility(PianoKeybd::ShowActivated);
    } else if(action == ui.actionAlways) {
        VPianoSettings::instance()->setNamesVisibility(PianoKeybd::ShowAlways);
    }
    ui.pianokeybd->setShowLabels(VPianoSettings::instance()->namesVisibility());
}

void VPiano::slotNameVariant(QAction* action)
{
    if(action == ui.actionSharps) {
        VPianoSettings::instance()->setNamesAlterations(PianoKeybd::ShowSharps);
    } else if(action == ui.actionFlats) {
        VPianoSettings::instance()->setNamesAlterations(PianoKeybd::ShowFlats);
    } else if(action == ui.actionNothing) {
        VPianoSettings::instance()->setNamesAlterations(PianoKeybd::ShowNothing);
    }
    ui.pianokeybd->setLabelAlterations(VPianoSettings::instance()->alterations());
}

void VPiano::slotNoteName(const QString& name)
{
    if (name.isEmpty()) {
        ui.statusBar->clearMessage();
    } else {
        ui.statusBar->showMessage(name);
    }
}
