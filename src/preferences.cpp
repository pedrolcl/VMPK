/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2020, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include "preferences.h"
#include "constants.h"
#include "vpiano.h"
#include "colordialog.h"
#include "vpianosettings.h"

#include <QPushButton>
#include <QShowEvent>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QDebug>

using namespace drumstick::widgets;

Preferences::Preferences(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi( this );
    ui.txtFileInstrument->setText(QSTR_DEFAULTINS);
    ui.txtFileKmap->setText(QSTR_DEFAULT);
    ui.txtFileRawKmap->setText(QSTR_DEFAULT);
    restoreDefaults();
    connect(ui.btnInstrument, &QPushButton::clicked, this, &Preferences::slotOpenInstrumentFile);
    connect(ui.btnColor, &QPushButton::clicked, this, &Preferences::slotSelectColor);
    connect(ui.btnKmap, &QPushButton::clicked, this, &Preferences::slotOpenKeymapFile);
    connect(ui.btnRawKmap, &QPushButton::clicked, this, &Preferences::slotOpenRawKeymapFile);
    connect(ui.btnFont, &QPushButton::clicked, this, &Preferences::slotSelectFont);
    QPushButton *btnDefaults = ui.buttonBox->button(QDialogButtonBox::RestoreDefaults);
    connect(btnDefaults, &QPushButton::clicked, this, &Preferences::slotRestoreDefaults);
    ui.cboStartingKey->clear();
    ui.cboColorPolicy->clear();
    ui.cboOctaveName->clear();
    VPianoSettings::instance()->initializePaletteStrings();
    ui.cboColorPolicy->addItems(VPianoSettings::instance()->availablePaletteNames(true));

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
    setMinimumSize(480,500);
    adjustSize();
#endif
}

void Preferences::showEvent ( QShowEvent *event )
{
    if (event->type() == QEvent::Show) {
        ui.spinNumKeys->setValue( VPianoSettings::instance()->numKeys() );
        ui.cboDrumsChannel->setCurrentIndex( VPianoSettings::instance()->drumsChannel() +1);
        ui.chkAlwaysOnTop->setChecked( VPianoSettings::instance()->alwaysOnTop() );
        ui.chkRawKeyboard->setChecked( VPianoSettings::instance()->rawKeyboard() );
        ui.chkVelocityColor->setChecked( VPianoSettings::instance()->velocityColor() );
        ui.chkEnforceChannelState->setChecked( VPianoSettings::instance()->enforceChannelState() );
        ui.chkEnableKeyboard->setChecked( VPianoSettings::instance()->enableKeyboard() );
        ui.chkEnableMouse->setChecked( VPianoSettings::instance()->enableMouse() );
        ui.chkEnableTouch->setChecked( VPianoSettings::instance()->enableTouch() );
        ui.cboColorPolicy->setCurrentIndex( VPianoSettings::instance()->highlightPaletteId() );
        ui.cboStartingKey->setCurrentIndex( ui.cboStartingKey->findData( VPianoSettings::instance()->startingKey() ) );
        ui.cboOctaveName->setCurrentIndex( VPianoSettings::instance()->namesOctave() );
        ui.txtFont->setText( VPianoSettings::instance()->namesFont().toString() );
    }
}

void Preferences::apply()
{
    VPianoSettings::instance()->setNumKeys( ui.spinNumKeys->value());
    VPianoSettings::instance()->setAlwaysOnTop( ui.chkAlwaysOnTop->isChecked() );
    VPianoSettings::instance()->setRawKeyboard( ui.chkRawKeyboard->isChecked() );
    VPianoSettings::instance()->setVelocityColor( ui.chkVelocityColor->isChecked() );
    VPianoSettings::instance()->setEnforceChannelState( ui.chkEnforceChannelState->isChecked() );
    VPianoSettings::instance()->setEnableKeyboard( ui.chkEnableKeyboard->isChecked() );
    VPianoSettings::instance()->setEnableMouse( ui.chkEnableMouse->isChecked() );
    VPianoSettings::instance()->setEnableTouch( ui.chkEnableTouch->isChecked() );
    if ( ui.txtFileRawKmap->text().isEmpty() ||
         ui.txtFileRawKmap->text() == QSTR_DEFAULT)
        VPianoSettings::instance()->setRawMapFile(QSTR_DEFAULT);
    if ( ui.txtFileKmap->text().isEmpty() ||
         ui.txtFileKmap->text() == QSTR_DEFAULT)
        VPianoSettings::instance()->setMapFile(QSTR_DEFAULT);
    if ( ui.txtFileInstrument->text().isEmpty() ||
         ui.txtFileInstrument->text() == QSTR_DEFAULTINS )
        VPianoSettings::instance()->setInsFileName( VPiano::dataDirectory() + QSTR_DEFAULTINS );
    VPianoSettings::instance()->setDrumsChannel( ui.cboDrumsChannel->currentIndex() - 1 );
    VPianoSettings::instance()->setHighlightPaletteId(ui.cboColorPolicy->currentIndex());
    VPianoSettings::instance()->setStartingKey( ui.cboStartingKey->itemData( ui.cboStartingKey->currentIndex()).toInt() );
    VPianoSettings::instance()->setNamesOctave(static_cast<LabelCentralOctave>(ui.cboOctaveName->currentIndex()));
    QFont f;
    QString fstr = ui.txtFont->text();
    if (fstr.isEmpty()) {
        fstr = QSTR_DEFAULTFONT;
    }
    if ( f.fromString(fstr) ) {
        VPianoSettings::instance()->setNamesFont( f );
    }
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
                                VPiano::dataDirectory(),
                                tr("Instrument definitions (*.ins)"));
    if (!fileName.isEmpty()) {
        setInstrumentsFileName(fileName);
    }
}

void Preferences::slotSelectColor()
{
    QPointer<ColorDialog> dlgColorPolicy = new ColorDialog(this);
    if (dlgColorPolicy != nullptr) {
        dlgColorPolicy->loadPalette(ui.cboColorPolicy->currentIndex());
        if(dlgColorPolicy->exec() == QDialog::Accepted) {
            int pal = dlgColorPolicy->selectedPalette();
            PianoPalette palette = VPianoSettings::instance()->getPalette(pal);
            if (palette.isHighLight()) {
                VPianoSettings::instance()->setHighlightPaletteId(pal);
                ui.cboColorPolicy->setCurrentIndex(pal);
            }
        }
    }
    delete dlgColorPolicy;
}

void Preferences::setInstrumentsFileName( const QString fileName )
{
    QFileInfo f(fileName);
    if (f.isReadable()) {
        VPianoSettings::instance()->setInstrumentsFileName( fileName );
        QStringList names = VPianoSettings::instance()->getInsNames();
        ui.txtFileInstrument->setText(f.fileName());
        ui.cboInstrument->clear();
        if (!names.isEmpty()) {
            ui.cboInstrument->addItems(names);
            ui.cboInstrument->setCurrentIndex(-1);
        }
    } else {
        VPianoSettings::instance()->setInsFileName(QString());
        ui.txtFileInstrument->setText(QString());
    }
}

void Preferences::setInstrumentName( const QString name )
{
    int index = ui.cboInstrument->findText( name );
    ui.cboInstrument->setCurrentIndex( index );
}

void Preferences::slotOpenKeymapFile()
{
    QString fileName = QFileDialog::getOpenFileName(0,
                                tr("Open keyboard map definition"),
                                VPiano::dataDirectory(),
                                tr("Keyboard map (*.xml)"));
    if (!fileName.isEmpty()) {
        setKeyMapFileName(fileName);
    }
}

void Preferences::slotOpenRawKeymapFile()
{
    QString fileName = QFileDialog::getOpenFileName(0,
                                tr("Open keyboard map definition"),
                                VPiano::dataDirectory(),
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
        VPianoSettings::instance()->setNamesFont(font);
        ui.txtFont->setText(font.toString());
    }
}

void Preferences::setRawKeyMapFileName( const QString fileName )
{
    VPianoSettings::instance()->setRawMapFile(fileName);
    ui.txtFileRawKmap->setText(VPianoSettings::instance()->getRawMapFile());
}

void Preferences::setKeyMapFileName( const QString fileName )
{
    VPianoSettings::instance()->setMapFile(fileName);
    ui.txtFileRawKmap->setText(VPianoSettings::instance()->getMapFile());
}

void Preferences::restoreDefaults()
{
    ui.chkAlwaysOnTop->setChecked(false);
    ui.chkRawKeyboard->setChecked(false);
    ui.spinNumKeys->setValue(DEFAULTNUMBEROFKEYS);
    ui.txtFileKmap->setText(QSTR_DEFAULT);
    ui.txtFileRawKmap->setText(QSTR_DEFAULT);
    ui.chkVelocityColor->setChecked(true);
    ui.chkEnforceChannelState->setChecked(false);
    ui.chkEnableKeyboard->setChecked(true);
    ui.chkEnableMouse->setChecked(true);
    ui.chkEnableTouch->setChecked(true);
    setInstrumentsFileName(VPiano::dataDirectory() + QSTR_DEFAULTINS);
    ui.cboInstrument->setCurrentIndex(0);
    ui.cboColorPolicy->setCurrentIndex(PAL_SINGLE);
    ui.cboStartingKey->setCurrentIndex(DEFAULTSTARTINGKEY);
    ui.txtFont->setText(QSTR_DEFAULTFONT);
}

void Preferences::slotRestoreDefaults()
{
    restoreDefaults();
}

void Preferences::retranslateUi()
{
    ui.retranslateUi(this);
    VPianoSettings::instance()->initializePaletteStrings();
}

void Preferences::setNoteNames(const QStringList& noteNames)
{
    ui.cboStartingKey->clear();
    for(int i=0; i<noteNames.length(); ++i) {
        int j = i;
        if (j >= 5) j++;
        if (j % 2 == 0) {
            ui.cboStartingKey->addItem(noteNames[i], i);
        }
    }
    ui.cboOctaveName->clear();
    for(int i=3; i<6; ++i) {
        ui.cboOctaveName->addItem(QString("%1%2").arg(noteNames[0]).arg(i));
    }
}
