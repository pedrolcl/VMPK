/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2024, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <http://www.gnu.org/licenses/>.
*/

#include <QPushButton>
#include <QShowEvent>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QStyleFactory>

#include "preferences.h"
#include "constants.h"
#include "colordialog.h"
#include "vpianosettings.h"

using namespace drumstick::widgets;

Preferences::Preferences(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi( this );
    ui.cboStartingKey->clear();
    ui.cboColorPolicy->clear();
    ui.cboOctaveName->clear();
    populateStyles();
    slotRestoreDefaults();
    connect(ui.btnInstrument, &QPushButton::clicked, this, &Preferences::slotOpenInstrumentFile);
    connect(ui.btnColor, &QPushButton::clicked, this, &Preferences::slotSelectColor);
    connect(ui.btnKmap, &QPushButton::clicked, this, &Preferences::slotOpenKeymapFile);
    connect(ui.btnRawKmap, &QPushButton::clicked, this, &Preferences::slotOpenRawKeymapFile);
    connect(ui.btnFont, &QPushButton::clicked, this, &Preferences::slotSelectFont);
    QPushButton *btnDefaults = ui.buttonBox->button(QDialogButtonBox::RestoreDefaults);
    connect(btnDefaults, &QPushButton::clicked, this, &Preferences::slotRestoreDefaults);
#if !defined(RAWKBD_SUPPORT)
    ui.chkRawKeyboard->setVisible(false);
    ui.lblRawKmap->setVisible(false);
    ui.txtFileRawKmap->setVisible(false);
    ui.btnRawKmap->setVisible(false);
#endif
#if defined(SMALL_SCREEN)
    ui.chkRawKeyboard->setVisible(false);
    ui.lblRawKmap->setVisible(false);
    ui.txtFileRawKmap->setVisible(false);
    ui.btnRawKmap->setVisible(false);
    ui.lblKmap->setVisible(false);
    ui.txtFileKmap->setVisible(false);
    ui.btnKmap->setVisible(false);
    ui.chkAlwaysOnTop->setVisible(false);
    ui.chkGrabKb->setVisible(false);
    ui.chkEnableKeyboard->setVisible(false);
    setWindowState(Qt::WindowActive | Qt::WindowMaximized);
#else
#if !defined(Q_OS_WINDOWS)
    ui.chkWinSnap->setVisible(false);
#endif
   adjustSize();
#endif
}

void Preferences::slotRestoreDefaults()
{
    ui.spinNumKeys->setValue(DEFAULTNUMBEROFKEYS);
    ui.cboStartingKey->setCurrentIndex( ui.cboStartingKey->findData( DEFAULTSTARTINGKEY ) );
    ui.cboColorPolicy->setCurrentIndex(0);
    setInstrumentsFileName(VPianoSettings::dataDirectory() + QSTR_DEFAULTINS);
    ui.cboInstrument->setCurrentIndex(0);
    ui.txtFileKmap->setText(m_mapFile = QSTR_DEFAULT);
    ui.txtFileRawKmap->setText(m_rawMapFile = QSTR_DEFAULT);
    ui.cboDrumsChannel->setCurrentIndex(MIDIGMDRUMSCHANNEL + 1);
    ui.cboOctaveName->setCurrentIndex(ui.cboOctaveName->findData(OctaveC4));
    m_font = qApp->font();
    m_font.setPointSize(50);
    ui.txtFont->setText( fontString(m_font) );
    ui.chkEnforceChannelState->setChecked(false);
    ui.chkVelocityColor->setChecked(true);
    ui.chkAlwaysOnTop->setChecked(false);
    ui.chkEnableKeyboard->setChecked(true);
    ui.chkRawKeyboard->setChecked(false);
    ui.chkEnableMouse->setChecked(true);
    ui.chkEnableTouch->setChecked(true);
#if defined(Q_OS_WINDOWS)
    ui.chkWinSnap->setEnabled(true);
#endif
    ui.chkDarkMode->setChecked(false);
    ui.cboStyle->setCurrentText(DEFAULTSTYLE);
}

void Preferences::showEvent ( QShowEvent *event )
{
    if (event->type() == QEvent::Show) {
        ui.spinNumKeys->setValue(VPianoSettings::instance()->numKeys());
        ui.cboStartingKey->setCurrentIndex(ui.cboStartingKey->findData(VPianoSettings::instance()->startingKey()));
        for(int i=0; i<VPianoSettings::instance()->availablePalettes(); ++i) {
            PianoPalette p = VPianoSettings::instance()->getPalette(i);
            if (p.isHighLight()) {
                ui.cboColorPolicy->addItem(p.paletteName(), p.paletteId());
                if (p.paletteId() == VPianoSettings::instance()->highlightPaletteId()) {
                    ui.cboColorPolicy->setCurrentText(p.paletteName());
                }
            }
        }
        setInstrumentsFileName(VPianoSettings::instance()->insFileName());
        ui.cboInstrument->setCurrentText( VPianoSettings::instance()->insName());
        setKeyMapFileName(VPianoSettings::instance()->getMapFile());
        setRawKeyMapFileName(VPianoSettings::instance()->getRawMapFile());
        ui.cboDrumsChannel->setCurrentIndex(VPianoSettings::instance()->drumsChannel()+1);
        ui.cboOctaveName->setCurrentIndex(ui.cboOctaveName->findData(VPianoSettings::instance()->namesOctave()));
        m_font = VPianoSettings::instance()->namesFont();
        ui.txtFont->setText( fontString(m_font) );
        ui.chkEnforceChannelState->setChecked( VPianoSettings::instance()->enforceChannelState() );
        ui.chkVelocityColor->setChecked( VPianoSettings::instance()->velocityColor() );
        ui.chkAlwaysOnTop->setChecked( VPianoSettings::instance()->alwaysOnTop() );
        ui.chkEnableKeyboard->setChecked( VPianoSettings::instance()->enableKeyboard() );
        ui.chkRawKeyboard->setChecked( VPianoSettings::instance()->rawKeyboard() );
        ui.chkEnableMouse->setChecked( VPianoSettings::instance()->enableMouse() );
        ui.chkEnableTouch->setChecked( VPianoSettings::instance()->enableTouch() );
        ui.cboStyle->setCurrentText(VPianoSettings::instance()->getStyle());
#if defined(Q_OS_WINDOWS)
        ui.chkWinSnap->setChecked( VPianoSettings::instance()->getWinSnap() );
#endif
        ui.chkDarkMode->setChecked( VPianoSettings::instance()->getDarkMode() );
    }
}

void Preferences::apply()
{
    VPianoSettings::instance()->setNumKeys(ui.spinNumKeys->value());
    VPianoSettings::instance()->setStartingKey(ui.cboStartingKey->currentData().toInt());
    VPianoSettings::instance()->setHighlightPaletteId(ui.cboColorPolicy->currentData().toInt());
    VPianoSettings::instance()->setInstruments(m_insFile, ui.cboInstrument->currentText());
    VPianoSettings::instance()->setMapFile( m_mapFile);
    VPianoSettings::instance()->setRawMapFile( m_rawMapFile );
    VPianoSettings::instance()->setDrumsChannel( ui.cboDrumsChannel->currentIndex() - 1 );
    VPianoSettings::instance()->setNamesOctave(static_cast<LabelCentralOctave>(ui.cboOctaveName->currentData().toInt()));
    VPianoSettings::instance()->setNamesFont( m_font );
    VPianoSettings::instance()->setEnforceChannelState( ui.chkEnforceChannelState->isChecked() );
    VPianoSettings::instance()->setVelocityColor( ui.chkVelocityColor->isChecked() );
    VPianoSettings::instance()->setAlwaysOnTop( ui.chkAlwaysOnTop->isChecked() );
    VPianoSettings::instance()->setEnableKeyboard( ui.chkEnableKeyboard->isChecked() );
    VPianoSettings::instance()->setRawKeyboard( ui.chkRawKeyboard->isChecked() );
    VPianoSettings::instance()->setEnableMouse( ui.chkEnableMouse->isChecked() );
    VPianoSettings::instance()->setEnableTouch( ui.chkEnableTouch->isChecked() );
    VPianoSettings::instance()->setStyle(ui.cboStyle->currentText());
#if defined(Q_OS_WINDOWS)
    VPianoSettings::instance()->setWinSnap( ui.chkWinSnap->isChecked() );
#endif
    VPianoSettings::instance()->setDarkMode( ui.chkDarkMode->isChecked() );
}

void Preferences::accept()
{
    apply();
    QDialog::accept();
}

void Preferences::slotOpenInstrumentFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("Open instruments definition"),
                                VPianoSettings::dataDirectory(),
                                tr("Instrument definitions (*.ins)"));
    if (!fileName.isEmpty()) {
        setInstrumentsFileName(fileName);
    }
}

void Preferences::slotSelectColor()
{
    QPointer<ColorDialog> dlgColorPolicy = new ColorDialog(true, this);
    if (dlgColorPolicy != nullptr) {
        dlgColorPolicy->loadPalette(ui.cboColorPolicy->currentData().toInt());
        if(dlgColorPolicy->exec() == QDialog::Accepted) {
            int pal = dlgColorPolicy->selectedPalette();
            PianoPalette palette = VPianoSettings::instance()->getPalette(pal);
            if (palette.isHighLight()) {
                ui.cboColorPolicy->setCurrentText(palette.paletteName());
            }
        }
    }
    delete dlgColorPolicy;
}

void Preferences::setInstrumentsFileName( const QString fileName )
{
    QFileInfo f(fileName);
    if (f.isReadable()) {
        m_insFile = f.absoluteFilePath();
        InstrumentList instruments;
        instruments.load(m_insFile);
        QStringList names;
        foreach(const Instrument& i, instruments.values()) {
            auto s = i.instrumentName();
            if (!s.endsWith(QLatin1String("Drums"), Qt::CaseInsensitive)) {
                names << s;
            }
        }
        ui.txtFileInstrument->setText(f.fileName());
        ui.cboInstrument->clear();
        if (!names.isEmpty()) {
            ui.cboInstrument->addItems(names);
        }
    } else {
        ui.txtFileInstrument->setText(QSTR_DEFAULT);
    }
}

void Preferences::setInstrumentName( const QString name )
{
    int index = ui.cboInstrument->findText( name );
    ui.cboInstrument->setCurrentIndex( index );
}

void Preferences::slotOpenKeymapFile()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,
                                tr("Open keyboard map definition"),
                                VPianoSettings::dataDirectory(),
                                tr("Keyboard map (*.xml)"));
    if (!fileName.isEmpty()) {
        setKeyMapFileName(fileName);
    }
}

void Preferences::slotOpenRawKeymapFile()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,
                                tr("Open keyboard map definition"),
                                VPianoSettings::dataDirectory(),
                                tr("Keyboard map (*.xml)"));
    if (!fileName.isEmpty()) {
        setRawKeyMapFileName(fileName);
    }
}

void Preferences::slotSelectFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok,
                    VPianoSettings::instance()->namesFont(),
                    this, tr("Font to display note names"),
                    QFontDialog::DontUseNativeDialog | QFontDialog::ScalableFonts);
    if (ok) {
        m_font = font;
        ui.txtFont->setText( fontString(font) );
    }
}

void Preferences::setRawKeyMapFileName( const QString fileName )
{
    QFileInfo f(fileName);
    if (f.isReadable()) {
        m_rawMapFile = f.absoluteFilePath();
        ui.txtFileRawKmap->setText(f.fileName());
    }
}

void Preferences::setKeyMapFileName( const QString fileName )
{
    QFileInfo f(fileName);
    if (f.isReadable()) {
        m_mapFile = f.absoluteFilePath();
        ui.txtFileKmap->setText(f.fileName());
    }
}

void Preferences::retranslateUi()
{
    ui.retranslateUi(this);
    VPianoSettings::instance()->retranslatePalettes();
}

void Preferences::setNoteNames(const QStringList& noteNames)
{
    ui.cboStartingKey->clear();
    for (int i=0; i<noteNames.length(); ++i) {
        int j = i;
        if (j >= 5) j++;
        if (j % 2 == 0) {
            ui.cboStartingKey->addItem(noteNames[i], i);
        }
    }

    ui.cboOctaveName->clear();
    ui.cboOctaveName->addItem(noteNames[0], OctaveNothing);
    ui.cboOctaveName->addItem(noteNames[0]+"3", OctaveC3);
    ui.cboOctaveName->addItem(noteNames[0]+"4", OctaveC4);
    ui.cboOctaveName->addItem(noteNames[0]+"5", OctaveC5);
}

void Preferences::populateStyles()
{
    ui.cboStyle->clear();
    QStringList styleNames = QStyleFactory::keys();
    ui.cboStyle->addItems(styleNames);
    QString currentStyle = qApp->style()->objectName();
    foreach(const QString& s, styleNames) {
        if (QString::compare(s, currentStyle, Qt::CaseInsensitive) == 0) {
            ui.cboStyle->setCurrentText(s);
            break;
        }
    }
}

QString Preferences::fontString(const QFont &f) const
{
    return QString("%1,%2").arg(f.family()).arg(f.pointSize());
}
