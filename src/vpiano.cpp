/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2024, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <QDebug>
#include <QDesktopServices>
#include <QInputDialog>
#include <QFileDialog>
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
#include <QMapIterator>
#include <QShortcut>
#include <QActionGroup>
#include <QStandardPaths>
#include <QTimer>

#include <drumstick/backendmanager.h>
#include <drumstick/rtmidiinput.h>
#include <drumstick/rtmidioutput.h>
#include <drumstick/pianokeybd.h>
#include <drumstick/settingsfactory.h>
#include <drumstick/configurationdialogs.h>

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

using namespace drumstick::rt;
using namespace drumstick::widgets;

VPiano::VPiano( QWidget * parent, Qt::WindowFlags flags )
    : QMainWindow(parent, flags),
    m_midiout(nullptr),
    m_midiin(nullptr),
    m_backendManager(nullptr),
    m_initialized(false),
#if defined(ENABLE_NATIVE_FILTER)	
    m_filter(nullptr),
#endif	
    m_currentLang(nullptr)
{
#if defined(ENABLE_DBUS)
    new VmpkAdaptor(this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject("/", this);
    dbus.registerService("net.sourceforge.vmpk");
#endif
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
    connect(ui.actionAbout, &QAction::triggered, this, &VPiano::slotAbout);
    connect(ui.actionAboutQt, &QAction::triggered, this, &VPiano::slotAboutQt);
    connect(ui.actionAboutTranslation, &QAction::triggered, this, &VPiano::slotAboutTranslation);
    connect(ui.actionConnections, &QAction::triggered, this, &VPiano::slotConnections);
    connect(ui.actionPreferences, &QAction::triggered, this, &VPiano::slotPreferences);
    connect(ui.actionEditKM, &QAction::triggered, this, &VPiano::slotEditKeyboardMap);
    connect(ui.actionContents, &QAction::triggered, this, &VPiano::slotHelpContents);
    connect(ui.actionWebSite, &QAction::triggered, this, &VPiano::slotOpenWebSite);
    connect(ui.actionImportSoundFont, &QAction::triggered, this, &VPiano::slotImportSF);
    connect(ui.actionEditExtraControls, &QAction::triggered, this, &VPiano::slotEditExtraControls);
    connect(ui.actionShortcuts, &QAction::triggered, this, &VPiano::slotShortcuts);
    connect(ui.actionKeyboardInput, &QAction::toggled, this, &VPiano::slotKeyboardInput);
    connect(ui.actionMouseInput, &QAction::toggled, this, &VPiano::slotMouseInput);
    connect(ui.actionTouchScreenInput, &QAction::toggled, this, &VPiano::slotTouchScreenInput);
    connect(ui.actionColorPalette, &QAction::triggered, this, &VPiano::slotColorPolicy);
    connect(ui.actionColorScale, &QAction::toggled, this, &VPiano::slotColorScale);
    connect(ui.actionWindowFrame, &QAction::toggled, this, &VPiano::toggleWindowFrame);
    connect(ui.actionOctave_Subscript_Designation,
            &QAction::toggled,
            this,
            &VPiano::slotOctaveSubscript);
    connect(ui.actionLoad_Configuration, &QAction::triggered, this, &VPiano::slotLoadConfiguration);
    connect(ui.actionSave_Configuration, &QAction::triggered, this, &VPiano::slotSaveConfiguration);
    // Toolbars actions: toggle view
    connect(ui.toolBarNotes->toggleViewAction(),
            &QAction::toggled,
            ui.actionNotes,
            &QAction::setChecked);
    connect(ui.toolBarControllers->toggleViewAction(),
            &QAction::toggled,
            ui.actionControllers,
            &QAction::setChecked);
    connect(ui.toolBarBender->toggleViewAction(),
            &QAction::toggled,
            ui.actionBender,
            &QAction::setChecked);
    connect(ui.toolBarPrograms->toggleViewAction(),
            &QAction::toggled,
            ui.actionPrograms,
            &QAction::setChecked);
    connect(ui.toolBarExtra->toggleViewAction(),
            &QAction::toggled,
            ui.actionExtraControls,
            &QAction::setChecked);
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
#if defined(RAWKBD_SUPPORT) && defined(ENABLE_NATIVE_FILTER)
    m_filter = new NativeFilter;
    m_filter->setRawKbdHandler(ui.pianokeybd);
    qApp->installNativeEventFilter(m_filter);
#endif
    initialization();
}

VPiano::~VPiano()
{
#if defined(RAWKBD_SUPPORT) && defined(ENABLE_NATIVE_FILTER)
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
        checkBackends();
    } else {
        qWarning() << "MIDI drivers are missing. Perhaps you need to set a DRUMSTICKRT environment variable?."
                   << "Unable to initialize MIDI. Terminating.";
    }
}

bool VPiano::initMidi()
{
    m_backendManager = new BackendManager();
    m_backendManager->refresh(VPianoSettings::instance()->settingsMap());

    m_midiin = m_backendManager->findInput(VPianoSettings::instance()->lastInputBackend());
    if (m_midiin == nullptr) {
        qWarning() << "MIDI IN driver not available";
    }

    m_midiout = m_backendManager->findOutput(VPianoSettings::instance()->lastOutputBackend());
    if (m_midiout == nullptr) {
        qWarning() << "MIDI OUT driver not available";
    }

    SettingsFactory settings;

    if (m_midiin != nullptr) {
        connectMidiInSignals();
        m_midiin->initialize(settings.getQSettings());
        MIDIConnection conn;
        auto connections = m_midiin->connections(VPianoSettings::instance()->advanced());
        auto lastConn = VPianoSettings::instance()->lastInputConnection();
        auto itr = std::find_if(connections.constBegin(), connections.constEnd(), [lastConn](const MIDIConnection& c){return c.first == lastConn;});
        if (itr == connections.constEnd()) {
            if (!connections.isEmpty()) {
                conn = connections.first();
            }
        } else {
            conn = (*itr);
        }
        m_midiin->open(conn);
        auto metaObj = m_midiin->metaObject();
        if ((metaObj->indexOfProperty("status") != -1) &&
            (metaObj->indexOfProperty("diagnostics") != -1)) {
            auto status = m_midiin->property("status");
            if (status.isValid() && !status.toBool()) {
                auto diagnostics = m_midiin->property("diagnostics");
                if (diagnostics.isValid()) {
                    auto text = diagnostics.toStringList().join(QChar::LineFeed).trimmed();
                    qWarning() << "MIDI Input" << text;
                }
            }
        }
    }

    if (m_midiout != nullptr) {
        m_midiout->initialize(settings.getQSettings());
        MIDIConnection conn;
        auto connections = m_midiout->connections(VPianoSettings::instance()->advanced());
        auto lastConn = VPianoSettings::instance()->lastOutputConnection();
        auto itr = std::find_if(connections.constBegin(), connections.constEnd(), [lastConn](const MIDIConnection& c){return c.first == lastConn;});
        if (itr == connections.constEnd()) {
            if (!connections.isEmpty()) {
                conn = connections.first();
            }
        } else {
            conn = (*itr);
        }
        m_midiout->open(conn);
        auto metaObj = m_midiout->metaObject();
        if ((metaObj->indexOfProperty("status") != -1) &&
            (metaObj->indexOfProperty("diagnostics") != -1)) {
            auto status = m_midiout->property("status");
            if (status.isValid() && !status.toBool()) {
                auto diagnostics = m_midiout->property("diagnostics");
                if (diagnostics.isValid()) {
                    auto text = diagnostics.toStringList().join(QChar::LineFeed).trimmed();
                    qWarning() << "MIDI Output" << text;
                }
            }
        }
        if (m_midiin != nullptr) {
            m_midiin->setMIDIThruDevice(m_midiout);
            m_midiin->enableMIDIThru(VPianoSettings::instance()->midiThru());
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
    connect(m_sboxChannel,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &VPiano::slotChannelValueChanged);
    connect(m_sboxOctave,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &VPiano::slotBaseOctaveValueChanged);
    connect(m_sboxTranspose,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &VPiano::slotTransposeValueChanged);
    connect(m_Velocity, &QAbstractSlider::valueChanged, this, &VPiano::slotVelocityValueChanged);
    connect(ui.actionChannelUp, &QAction::triggered, m_sboxChannel, &QSpinBox::stepUp);
    connect(ui.actionChannelDown, &QAction::triggered, m_sboxChannel, &QSpinBox::stepDown);
    connect(ui.actionOctaveUp, &QAction::triggered, m_sboxOctave, &QSpinBox::stepUp);
    connect(ui.actionOctaveDown, &QAction::triggered, m_sboxOctave, &QSpinBox::stepDown);
    connect(ui.actionTransposeUp, &QAction::triggered, m_sboxTranspose, &QSpinBox::stepUp);
    connect(ui.actionTransposeDown, &QAction::triggered, m_sboxTranspose, &QSpinBox::stepDown);
    connect(ui.actionVelocityUp, &QAction::triggered, this, &VPiano::slotVelocityUp);
    connect(ui.actionVelocityDown, &QAction::triggered, this, &VPiano::slotVelocityDown);
    // Controllers tool bar
    m_lblControl = new QLabel(this);
    ui.toolBarControllers->addWidget(m_lblControl);
    m_lblControl ->setMargin(TOOLBARLABELMARGIN);
    m_comboControl = new QComboBox(this);
    m_comboControl->setObjectName("cboControl");
    //m_comboControl->setStyleSheet("combobox-popup: 0;");
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
    connect(m_comboControl,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &VPiano::slotComboControlCurrentIndexChanged);
    connect(m_Control, &QAbstractSlider::valueChanged, this, &VPiano::slotControlValueChanged);
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
    connect(m_bender, &QAbstractSlider::sliderMoved, this, &VPiano::slotBenderSliderMoved);
    connect(m_bender, &QAbstractSlider::sliderReleased, this, &VPiano::slotBenderSliderReleased);
    // Programs tool bar
    m_lblBank = new QLabel(this);
    ui.toolBarPrograms->addWidget(m_lblBank);
    m_lblBank->setMargin(TOOLBARLABELMARGIN);
    m_comboBank = new QComboBox(this);
    m_comboBank->setObjectName("cboBank");
    m_comboBank->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    //m_comboBank->setStyleSheet("combobox-popup: 0;");
    m_comboBank->setFocusPolicy(Qt::NoFocus);
    ui.toolBarPrograms->addWidget(m_comboBank);
    m_lblProgram = new QLabel(this);
    ui.toolBarPrograms->addWidget(m_lblProgram);
    m_lblProgram->setMargin(TOOLBARLABELMARGIN);
    m_comboProg = new QComboBox(this);
    m_comboProg->setObjectName("cboProg");
    m_comboProg->setStyleSheet("combobox-popup: 0;");
    m_comboProg->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_comboProg->setFocusPolicy(Qt::NoFocus);
    ui.toolBarPrograms->addWidget(m_comboProg);
    connect(m_comboBank,
            QOverload<int>::of(&QComboBox::activated),
            this,
            &VPiano::slotComboBankActivated);
    connect(m_comboProg,
            QOverload<int>::of(&QComboBox::activated),
            this,
            &VPiano::slotComboProgActivated);
    // Toolbars actions: buttons
    connect(ui.actionPanic, &QAction::triggered, this, &VPiano::slotPanic);
    connect(ui.actionResetAll, &QAction::triggered, this, &VPiano::slotResetAllControllers);
    connect(ui.actionReset, &QAction::triggered, this, &VPiano::slotResetBender);
    connect(ui.actionEditExtra, &QAction::triggered, this, &VPiano::slotEditExtraControls);
    // Tools actions
    connect(ui.actionNextBank, &QAction::triggered, this, &VPiano::slotBankNext);
    connect(ui.actionPreviousBank, &QAction::triggered, this, &VPiano::slotBankPrev);
    connect(ui.actionNextProgram, &QAction::triggered, this, &VPiano::slotProgramNext);
    connect(ui.actionPreviousProgram, &QAction::triggered, this, &VPiano::slotProgramPrev);
    connect(ui.actionNextController, &QAction::triggered, this, &VPiano::slotControllerNext);
    connect(ui.actionPreviousController, &QAction::triggered, this, &VPiano::slotControllerPrev);
    connect(ui.actionControllerDown, &QAction::triggered, this, &VPiano::slotControllerDown);
    connect(ui.actionControllerUp, &QAction::triggered, this, &VPiano::slotControllerUp);
    /* connect( ui.actionEditPrograms, &QAction::triggered, this, &VPiano::slotEditPrograms); */
    retranslateToolbars();
}

/*void VPiano::slotDebugDestroyed(QObject *obj)
{
    qDebug() << Q_FUNC_INFO << obj->metaObject()->className();
}*/

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
        for (int ch = 0; ch < MIDICHANNELS; ++ch) {
            if (!m_ctlState[ch].contains(control)) {
                m_ctlState[ch][control] = defValue;
            }
        }
        if (m_ctlState[channel].contains(control))
            value = m_ctlState[channel][control];
        else
            value = defValue;
        switch (type) {
        case ExtraControl::ControlType::SwitchControl:
            chkbox = new QCheckBox(this);
            chkbox->setStyleSheet(QSTR_CHKBOXSTYLE);
            chkbox->setProperty(MIDICTLONVALUE, maxValue);
            chkbox->setProperty(MIDICTLOFFVALUE, minValue);
            chkbox->setChecked(bool(value));
            chkbox->setAutoRepeat(false);
            connect(chkbox, &QCheckBox::clicked, this, &VPiano::slotControlClicked);
            if (!keySequence.isEmpty()) {
                QShortcut *s = new QShortcut(keySequence, this);
                s->setAutoRepeat(false);
                connect(s, &QShortcut::activated, chkbox, [=] { chkbox->animateClick(); });
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
            connect(knob, &QAbstractSlider::valueChanged, this, &VPiano::slotExtraController);
            w = knob;
            break;
        case ExtraControl::ControlType::SpinBoxControl:
            spin = new QSpinBox(this);
            spin->setMinimum(minValue);
            spin->setMaximum(maxValue);
            spin->setValue(value);
            connect(spin,
                    QOverload<int>::of(&QSpinBox::valueChanged),
                    this,
                    &VPiano::slotExtraController);
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
            connect(slider, &QAbstractSlider::valueChanged, this, &VPiano::slotExtraController);
            w = slider;
            break;
        case ExtraControl::ControlType::ButtonCtlControl:
            button = new QToolButton(this);
            button->setText(lbl);
            button->setProperty(MIDICTLONVALUE, maxValue);
            button->setProperty(MIDICTLOFFVALUE, minValue);
            button->setAutoRepeat(false);
            connect(button, &QToolButton::clicked, this, &VPiano::slotControlClicked);
            if (!keySequence.isEmpty()) {
                QShortcut *s = new QShortcut(keySequence, this);
                s->setAutoRepeat(false);
                connect(s, &QShortcut::activated, button, [=] { button->animateClick(); });
            }
            w = button;
            break;
        case ExtraControl::ControlType::ButtonSyxControl:
            control = 255;
            button = new QToolButton(this);
            button->setText(lbl);
            button->setProperty(SYSEXFILENAME, fileName);
            button->setProperty(SYSEXFILEDATA, readSysexDataFile(fileName));
            button->setAutoRepeat(false);
            connect(button, &QToolButton::clicked, this, &VPiano::slotControlClicked);
            if (!keySequence.isEmpty()) {
                QShortcut *s = new QShortcut(keySequence, this);
                s->setAutoRepeat(false);
                connect(s, &QShortcut::activated, button, [=] { button->animateClick(); });
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
                //connect(qlbl, &QObject::destroyed, this, &VPiano::slotDebugDestroyed);
            }
            w->setProperty(MIDICTLNUMBER, control);
            w->setProperty(MIDICTLDEFVALUE, defValue);
            w->setFocusPolicy(Qt::NoFocus);
            ui.toolBarExtra->addWidget(w);
            //connect(w, &QObject::destroyed, this, &VPiano::slotDebugDestroyed);
        }
    }
}

void VPiano::readSettings()
{
    VPianoSettings::instance()->retranslatePalettes();

    ui.actionStatusBar->setChecked(VPianoSettings::instance()->showStatusBar());
    ui.statusBar->setVisible(VPianoSettings::instance()->showStatusBar());
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

    switch(VPianoSettings::instance()->namesVisibility()) {
    case ShowNever:
        ui.actionNever->setChecked(true);
        break;
    case ShowMinimum:
        ui.actionMinimal->setChecked(true);
        break;
    case ShowActivated:
        ui.actionWhen_Activated->setChecked(true);
        break;
    case ShowAlways:
        ui.actionAlways->setChecked(true);
        break;
    }
    ui.pianokeybd->setShowLabels(VPianoSettings::instance()->namesVisibility());

    switch(VPianoSettings::instance()->alterations()) {
    case ShowSharps:
        ui.actionSharps->setChecked(true);
        break;
    case ShowFlats:
        ui.actionFlats->setChecked(true);
        break;
    case ShowNothing:
        ui.actionNothing->setChecked(true);
        break;
    }
    ui.pianokeybd->setLabelAlterations(VPianoSettings::instance()->alterations());

    switch(VPianoSettings::instance()->namesOrientation()) {
    case HorizontalOrientation:
        ui.actionHorizontal->setChecked(true);
        break;
    case VerticalOrientation:
        ui.actionVertical->setChecked(true);
        break;
    case AutomaticOrientation:
        ui.actionAutomatic->setChecked(true);
        break;
    }
    ui.pianokeybd->setLabelOrientation(VPianoSettings::instance()->namesOrientation());
    ui.pianokeybd->setLabelOctave(VPianoSettings::instance()->namesOctave());
    ui.actionOctave_Subscript_Designation->setChecked(VPianoSettings::instance()->octaveSubscript());
    ui.pianokeybd->setOctaveSubscript(VPianoSettings::instance()->octaveSubscript());

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
    if (m_extraControls.isEmpty()) {
        QString softPedal = tr("Soft") + ",67,0,0,64,0,";
        QString sostenutoPedal = tr("Sostenuto") + ",66,0,0,64,0,";
        QString sustainPedal = tr("Sustain") + ",64,0,0,64,0,";
        m_extraControls << softPedal << sostenutoPedal << sustainPedal;
    }
}

void VPiano::writeSettings()
{
    VPianoSettings::instance()->setShowStatusBar(ui.actionStatusBar->isChecked());
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

    writeDriverSettings(settings.getQSettings(), m_midiin);
    writeDriverSettings(settings.getQSettings(), m_midiout);

    settings->sync();
}

void VPiano::closeEvent( QCloseEvent *event )
{
    //qDebug() << Q_FUNC_INFO;
    if (m_initialized) {
        writeSettings();
    }
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

QColor VPiano::getHighlightColorFromPolicy(const int chan, const int note, const int vel)
{
    Q_UNUSED(vel)
    PianoPalette palette = VPianoSettings::instance()->getPalette(VPianoSettings::instance()->highlightPaletteId());
    switch (palette.paletteId()) {
    case PAL_SINGLE:
        return palette.getColor(0);
    case PAL_DOUBLE:
        return palette.getColor(getType(note));
    case PAL_CHANNELS:
        return palette.getColor(chan);
    case PAL_HISCALE:
        return palette.getColor(getDegree(note));
    default:
        break;
    }
    return QColor();
}

void VPiano::slotNoteOn(const int chan, const int note, const int vel)
{
    if (VPianoSettings::instance()->channel() == chan || VPianoSettings::instance()->omniMode()) {
        if (vel == 0) {
            slotNoteOff(chan, note, vel);
        } else {
            QColor c = getHighlightColorFromPolicy(chan, note, vel);
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
    //qDebug() << Q_FUNC_INFO << firstTime;
    std::call_once(m_firstTime, [=] {
        if (!restoreGeometry(VPianoSettings::instance()->geometry())) {
            qWarning() << "restoreGeometry() failed!";
        }
        if (!restoreState(VPianoSettings::instance()->state())) {
            qWarning() << "restoreState() failed!";
        }
    });
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
    initializeAllControllers();
    sendController(CTL_RESET_ALL_CTL, 0);
}

void VPiano::initializeAllControllers()
{
    int channel = VPianoSettings::instance()->channel();
    initControllers(channel);
    int index = m_comboControl->currentIndex();
    int ctl = m_comboControl->itemData(index).toInt();
    int val = m_ctlState[channel][ctl];
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
                auto v = w->property(MIDICTLDEFVALUE);
                if (v.isValid()) {
                    val = v.toInt();
                } else {
                    val = m_ctlState[channel][ctl];
                }
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

void VPiano::slotControlValueChanged(const int value)
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

void VPiano::connectMidiInSignals()
{
    if (m_midiin != nullptr) {
        connect(m_midiin, &MIDIInput::midiNoteOn, this, &VPiano::slotNoteOn, Qt::QueuedConnection);
        connect(m_midiin, &MIDIInput::midiNoteOff, this, &VPiano::slotNoteOff, Qt::QueuedConnection);
        connect(m_midiin, &MIDIInput::midiKeyPressure, this, &VPiano::slotKeyPressure, Qt::QueuedConnection);
        connect(m_midiin, &MIDIInput::midiChannelPressure, this, &VPiano::slotChannelPressure, Qt::QueuedConnection);
        connect(m_midiin, &MIDIInput::midiController, this, &VPiano::slotController, Qt::QueuedConnection);
        connect(m_midiin, &MIDIInput::midiProgram, this, &VPiano::slotProgram, Qt::QueuedConnection);
        connect(m_midiin, &MIDIInput::midiPitchBend, this, &VPiano::slotPitchBend, Qt::QueuedConnection);
    }
}

void VPiano::writeDriverSettings(QSettings *settings, QObject *driver)
{
    if (settings != nullptr && driver != nullptr) {
        const QMetaObject *mo = driver->metaObject();
        //const auto signature = QMetaObject::normalizedSignature("writeSettings(QSettings*)");
        const auto index = mo->indexOfMethod("writeSettings(QSettings*)");
        if (index != -1) {
            /*auto res =*/
            QMetaObject::invokeMethod(driver, "writeSettings", Q_ARG(QSettings *, settings));
            //qDebug() << Q_FUNC_INFO << mo->className() << res;
        }
    }
}

void VPiano::slotConnections()
{
    QPointer<MidiSetup> dlgMidiSetup = new MidiSetup(this);
    dlgMidiSetup->setInputs(m_backendManager->availableInputs());
    dlgMidiSetup->setOutputs(m_backendManager->availableOutputs());
    dlgMidiSetup->setInput(m_midiin);
    dlgMidiSetup->setOutput(m_midiout);
    releaseKb();
    if (dlgMidiSetup->exec() == QDialog::Accepted) {
        if (m_midiin != nullptr) {
            m_midiin->disconnect();
        }
        if (m_midiout != nullptr) {
            m_midiout->disconnect();
        }
        m_midiin = dlgMidiSetup->getInput();
        m_midiout = dlgMidiSetup->getOutput();
        connectMidiInSignals();
        enforceMIDIChannelState();
    }
    grabKb();
    delete dlgMidiSetup;
}

void VPiano::initControllers(int channel)
{
    if (m_ins != nullptr) {
        InstrumentData controls = m_ins->control();
        InstrumentData::ConstIterator it;
        for (it = controls.constBegin(); it != controls.constEnd(); ++it) {
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
    static QPalette defaultPalette = qApp->palette();
    static QPalette darkPalette(QColor(0x30,0x30,0x30));

    ui.pianokeybd->allKeysOff();
    ui.pianokeybd->setFont(VPianoSettings::instance()->namesFont());
    ui.pianokeybd->setLabelOctave(VPianoSettings::instance()->namesOctave());
    if ( ui.pianokeybd->numKeys() != VPianoSettings::instance()->numKeys() ||
         ui.pianokeybd->startKey() != VPianoSettings::instance()->startingKey() )
    {
        ui.pianokeybd->setNumKeys(VPianoSettings::instance()->numKeys(), VPianoSettings::instance()->startingKey());
    }
#if defined(RAWKBD_SUPPORT) && defined(ENABLE_NATIVE_FILTER)
    m_filter->setRawKbdEnabled(VPianoSettings::instance()->rawKeyboard());
    ui.pianokeybd->setUsingNativeFilter(VPianoSettings::instance()->rawKeyboard());
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

#if defined(Q_OS_WINDOWS)
    m_snapper.SetEnabled(VPianoSettings::instance()->getWinSnap());
#endif
    qApp->setPalette( VPianoSettings::instance()->getDarkMode() ? darkPalette : defaultPalette );
    qApp->setStyle( VPianoSettings::instance()->getStyle() );

	if (VPianoSettings::instance()->rawKeyboard()) {
		VMPKKeyboardMap* map = VPianoSettings::instance()->getRawKeyboardMap();
		if (!map->getFileName().isEmpty() && map->getFileName() != QSTR_DEFAULT ) {
			ui.pianokeybd->setRawKeyboardMap(map);
		} else {
			ui.pianokeybd->resetRawKeyboardMap();
		}
	} else {
		VMPKKeyboardMap* map = VPianoSettings::instance()->getKeyboardMap();
		if (!map->getFileName().isEmpty() && map->getFileName() != QSTR_DEFAULT ) {
			ui.pianokeybd->setKeyboardMap(map);
		} else {
			ui.pianokeybd->resetKeyboardMap();
		}
	}
    PianoPalette highlight = VPianoSettings::instance()->getPalette(VPianoSettings::instance()->highlightPaletteId());
    ui.pianokeybd->setHighlightPalette(highlight);
    PianoPalette background = VPianoSettings::instance()->getPalette(VPianoSettings::instance()->colorScale() ? PAL_SCALE : PAL_KEYS);
    ui.pianokeybd->setBackgroundPalette(background);
    ui.pianokeybd->setForegroundPalette(VPianoSettings::instance()->getPalette(PAL_FONT));
    ui.pianokeybd->setShowColorScale(VPianoSettings::instance()->colorScale());

    populateInstruments();
    populateControllers();

    QPoint wpos = pos();
    Qt::WindowFlags flags = windowFlags();
    flags.setFlag( Qt::WindowStaysOnTopHint, VPianoSettings::instance()->alwaysOnTop() );
    setWindowFlags( flags );
    move(wpos);
    show();
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
    dlgPreferences->setNoteNames(ui.pianokeybd->standardNoteNames());
    releaseKb();
    if (dlgPreferences->exec() == QDialog::Accepted) {
        applyPreferences();
    }
    delete dlgPreferences;
    grabKb();
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
        allNotesOff();
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
        allNotesOff();
        VPianoSettings::instance()->setChannel(c);
        ui.pianokeybd->setChannel(c);
        if (updDrums) {
            populateInstruments();
            populateControllers();
            updateNoteNames(c == drms);
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

void VPiano::removeExtraController(int ctl)
{
    auto controls = m_ins->control();
    if (!controls.contains(ctl)) {
        for (int ch = 0; ch < MIDICHANNELS; ++ch) {
            if (m_ctlState[ch].contains(ctl)) {
                m_ctlState[ch].remove(ctl);
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
#if defined(RAWKBD_SUPPORT) && defined(ENABLE_NATIVE_FILTER)
    m_filter->setRawKbdEnabled(VPianoSettings::instance()->rawKeyboard());
    ui.pianokeybd->setUsingNativeFilter(VPianoSettings::instance()->rawKeyboard());
#endif
}

void VPiano::releaseKb()
{
#if defined(RAWKBD_SUPPORT) && defined(ENABLE_NATIVE_FILTER)
    m_filter->setRawKbdEnabled(false);
    ui.pianokeybd->setUsingNativeFilter(false);
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
        connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::close);
    }
};
#endif

void VPiano::slotHelpContents()
{
    QStringList hlps;
    QLocale loc(VPianoSettings::instance()->language());
    QStringList lc = loc.name().split("_");
    hlps += QSTR_HELPL.arg(loc.name());
    if (lc.count() > 1)
        hlps += QSTR_HELPL.arg(lc[0]);
    hlps += QSTR_HELP;
    QDir hlpDir(VPianoSettings::dataDirectory());
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
        VPianoSettings::instance()->setInstruments(dlgRiffImport->getOutput(), dlgRiffImport->getName());
        //set the soundfont in the output driver preferences:
        drumstick::widgets::changeSoundFont(VPianoSettings::instance()->lastOutputBackend(), dlgRiffImport->getInput(), this);
        applyPreferences();
    }
    grabKb();
    delete dlgRiffImport;
}

void VPiano::slotEditExtraControls()
{
    QPointer<DialogExtraControls> dlgExtra = new DialogExtraControls(this);
    connect(dlgExtra, &DialogExtraControls::controlRemoved, this, &VPiano::removeExtraController);
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

void VPiano::checkBackends()
{
#if defined(Q_OS_LINUX)
    const QString output{QLatin1String("SonivoxEAS")};
#elif defined(Q_OS_MACOS)
    const QString output{QLatin1String("DLS Synth")};
#elif defined(Q_OS_WINDOWS)
    const QString output{QLatin1String("Windows MM")};
#elif defined(Q_OS_UNIX)
    const QString output{QLatin1String("FluidSynth")};
#else
    const QString output{QLatin1String("Network")};;
#endif

    if (m_backendManager != nullptr) {
        auto outs = m_backendManager->availableOutputs();
        bool found = false;
        foreach(const auto obj, outs) {
            if (obj->backendName() == output) {
                found = true;
                break;
            }
        }
        if (!found) {
            QTimer::singleShot(1500, this, [=](){slotBackendTest(output);});
        }
    }
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
    if( m_midiin != nullptr && m_midiout != nullptr) {
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
}

void VPiano::slotControllerDown()
{
    m_Control->triggerAction(QDial::SliderPageStepSub);
}

void VPiano::slotSwitchLanguage(QAction *action)
{
    QString lang = action->data().toString();
    bool result = QMessageBox::question (this, tr("Language Changed"),
            tr("The language for this application is going to change to %1. "
               "Do you want to continue?").arg(m_supportedLangs[lang]),
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
    if ( result )
    {
        VPianoSettings::instance()->setLanguage(lang);
        retranslateUi();
    } else {
        if (m_currentLang == nullptr) {
            m_currentLang = action;
        }
        m_currentLang->setChecked(true);
    }
}

void VPiano::createLanguageMenu()
{
    QString currentLang = VPianoSettings::instance()->language();
    if (currentLang.isEmpty()) {
        QLocale locale;
        if (locale.language() == QLocale::C || locale.language() == QLocale::English)
            currentLang = "C";
        else
            currentLang = locale.name().left(2);
    }
    QActionGroup *languageGroup = new QActionGroup(this);
    languageGroup->setExclusive(true);
    connect(languageGroup, &QActionGroup::triggered, this, &VPiano::slotSwitchLanguage, Qt::QueuedConnection);
    QDir dir(VPianoSettings::localeDirectory());
    QStringList fileNames = dir.entryList(QStringList(QSTR_VMPKPX + "*.qm"));
    QStringList locales;
    locales << "C";
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
    QString currentLang = VPianoSettings::instance()->language();
    bool supported(false);
    if (!currentLang.isEmpty() && (currentLang != "C")) {
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
    VPianoSettings::instance()->loadTranslations();
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
    m_supportedLangs.insert(QLatin1String("C"), tr("English"));
    m_supportedLangs.insert(QLatin1String("cs"), tr("Czech"));
    m_supportedLangs.insert(QLatin1String("de"), tr("German"));
    m_supportedLangs.insert(QLatin1String("es"), tr("Spanish"));
    m_supportedLangs.insert(QLatin1String("fr"), tr("French"));
    m_supportedLangs.insert(QLatin1String("gl"), tr("Galician"));
    m_supportedLangs.insert(QLatin1String("it"), tr("Italian"));
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
        QMap<int,int>::Iterator i, end;
        i = m_ctlState[channel].begin();
        end = m_ctlState[channel].end();
        for (; i != end; ++i) {
            sendController(i.key(), i.value());
        }
        if (m_lastBank[channel] < 0) {
            m_lastBank[channel] = 0;
        }
        sendBankChange(m_lastBank[channel]);
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
    QPointer<ColorDialog> dlgColorPolicy = new ColorDialog(false, this);
    dlgColorPolicy->loadPalette(VPianoSettings::instance()->highlightPaletteId());
    if (dlgColorPolicy->exec() == QDialog::Accepted) {
        int pal = dlgColorPolicy->selectedPalette();
        PianoPalette editedPalette = VPianoSettings::instance()->getPalette(pal);
        if (editedPalette.isHighLight()) {
            VPianoSettings::instance()->setHighlightPaletteId(pal);
            ui.pianokeybd->setHighlightPalette(editedPalette);
        } else if (editedPalette.isBackground()) {
            if ((VPianoSettings::instance()->colorScale() && (editedPalette.paletteId() == PAL_SCALE)) ||
                (!VPianoSettings::instance()->colorScale() && (editedPalette.paletteId() == PAL_KEYS))) {
                ui.pianokeybd->setBackgroundPalette(editedPalette);
            }
        } else if (editedPalette.isForeground()) {
            ui.pianokeybd->setForegroundPalette(editedPalette);
        }
    }
    delete dlgColorPolicy;
}

void VPiano::slotColorScale(bool value)
{
    if (value != VPianoSettings::instance()->colorScale()) {
        VPianoSettings::instance()->setColorScale(value);
        ui.pianokeybd->setBackgroundPalette(VPianoSettings::instance()->getPalette(value ? PAL_SCALE : PAL_KEYS));
        ui.pianokeybd->setShowColorScale(value);
    }
}

void VPiano::toggleWindowFrame(const bool state)
{
    QPoint wpos = pos();
    Qt::WindowFlags flags = windowFlags();
    flags.setFlag( Qt::FramelessWindowHint, !state );
    setWindowFlags(flags);
    move(wpos);
    show();
}

void VPiano::slotNameOrientation(QAction* action)
{
    if(action == ui.actionHorizontal) {
        VPianoSettings::instance()->setNamesOrientation(HorizontalOrientation);
    } else if(action == ui.actionVertical) {
        VPianoSettings::instance()->setNamesOrientation(VerticalOrientation);
    } else if(action == ui.actionAutomatic) {
        VPianoSettings::instance()->setNamesOrientation(AutomaticOrientation);
    }
    ui.pianokeybd->setLabelOrientation(VPianoSettings::instance()->namesOrientation());
}

void VPiano::slotNameVisibility(QAction* action)
{
    if(action == ui.actionNever) {
        VPianoSettings::instance()->setNamesVisibility(ShowNever);
    } else if(action == ui.actionMinimal) {
        VPianoSettings::instance()->setNamesVisibility(ShowMinimum);
    } else if(action == ui.actionWhen_Activated) {
        VPianoSettings::instance()->setNamesVisibility(ShowActivated);
    } else if(action == ui.actionAlways) {
        VPianoSettings::instance()->setNamesVisibility(ShowAlways);
    }
    ui.pianokeybd->setShowLabels(VPianoSettings::instance()->namesVisibility());
}

void VPiano::slotNameVariant(QAction* action)
{
    if(action == ui.actionSharps) {
        VPianoSettings::instance()->setNamesAlterations(ShowSharps);
    } else if(action == ui.actionFlats) {
        VPianoSettings::instance()->setNamesAlterations(ShowFlats);
    } else if(action == ui.actionNothing) {
        VPianoSettings::instance()->setNamesAlterations(ShowNothing);
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

void VPiano::slotLoadConfiguration()
{
    releaseKb();
    auto configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Configuration File"),
        configDir, tr("Configuration files (*.conf *.ini)"));
    if (!fileName.isEmpty()) {
        VPianoSettings::instance()->ReadFromFile(fileName);
        qApp->exit(RESTART_VMPK);
    }
    grabKb();
}

void VPiano::slotSaveConfiguration()
{
    releaseKb();
    const auto configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QFileDialog dlg(this);
    dlg.setNameFilter(tr("Configuration files (*.conf *.ini)"));
    dlg.setDirectory(configDir);
    dlg.setWindowTitle(tr("Save Configuration File"));
    dlg.setDefaultSuffix("conf");
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    if (dlg.exec() == QDialog::Accepted) {
        const auto selected = dlg.selectedFiles();
        if (!selected.isEmpty()) {
            const auto lastFileName = SettingsFactory::fileName();
            SettingsFactory::setFileName(selected.constFirst());
            writeSettings();
            SettingsFactory::setFileName(lastFileName);
        }
    }
    grabKb();
}

void VPiano::slotOctaveSubscript(bool value)
{
    //qDebug() << Q_FUNC_INFO << value;
    VPianoSettings::instance()->setOctaveSubscript(value);
    ui.pianokeybd->setOctaveSubscript(value);
}

void VPiano::slotBackendTest(const QString &name)
{
    //qDebug() << Q_FUNC_INFO << name;
    QString msg = tr("The important component called '%1' is missing.\n"
      "This probably means that your installation is corrupt, or "
      "the 'drumstick' packages on which this program depends are "
      "incomplete, outdated or wrong. You may still use the program, "
      "but probably without sound.").arg(name);
    QMessageBox::warning(this, tr("MIDI Output"), msg);
}

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
bool VPiano::nativeEvent(const QByteArray &eventType, void *message, long *result)
#else
bool VPiano::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
#endif
{
#if defined(Q_OS_WINDOWS)
    if (VPianoSettings::instance()->getWinSnap() && m_snapper.HandleMessage(message)) {
        result = 0;
        return true;
    }
#endif
    return QWidget::nativeEvent(eventType, message, result);
}

void VPiano::changeEvent(QEvent *event)
{
#if defined(Q_OS_WINDOWS)
    if (event->type() == QEvent::PaletteChange) {
        foreach(QToolBar *tb, findChildren<QToolBar*>()) {
            foreach(QComboBox *cb, tb->findChildren<QComboBox*>()) {
                cb->setPalette(qApp->palette());
                foreach(QWidget *w, cb->findChildren<QWidget*>()) {
                    w->setPalette(qApp->palette());
                }
                //qDebug() << cb;
            }
        }
    }
#endif
    QMainWindow::changeEvent(event);
}
