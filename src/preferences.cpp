/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2018, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <QPushButton>
#include <QShowEvent>
#include <QFileDialog>
#include <QColorDialog>
#include <QDebug>

Preferences::Preferences(QWidget *parent)
    : QDialog(parent),
    m_numKeys(DEFAULTNUMBEROFKEYS),
    m_drumsChannel(MIDIGMDRUMSCHANNEL),
    m_alwaysOnTop(false),
    m_rawKeyboard(false),
    m_velocityColor(true),
    m_enforceChannelState(false),
    m_enableKeyboard(true),
    m_enableMouse(true),
    m_enableTouch(true),
    m_colorDialog(0)
{
    ui.setupUi( this );
    ui.txtFileInstrument->setText(QSTR_DEFAULT);
    ui.txtFileKmap->setText(QSTR_DEFAULT);
    ui.txtFileRawKmap->setText(QSTR_DEFAULT);
    m_keymap.setRawMode(false);
    m_rawmap.setRawMode(true);
    restoreDefaults();
    connect(ui.btnInstrument, SIGNAL(clicked()), SLOT(slotOpenInstrumentFile()));
    connect(ui.btnColor, SIGNAL(clicked()), SLOT(slotSelectColor()));
    connect(ui.btnKmap, SIGNAL(clicked()), SLOT(slotOpenKeymapFile()));
    connect(ui.btnRawKmap, SIGNAL(clicked()), SLOT(slotOpenRawKeymapFile()));
    QPushButton *btnDefaults = ui.buttonBox->button(QDialogButtonBox::RestoreDefaults);
    connect(btnDefaults, SIGNAL(clicked()), SLOT(slotRestoreDefaults()));
    ui.cboStartingKey->clear();

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
        ui.spinNumKeys->setValue( m_numKeys );
        ui.cboDrumsChannel->setCurrentIndex(m_drumsChannel+1);
        ui.chkAlwaysOnTop->setChecked( m_alwaysOnTop );
        ui.chkRawKeyboard->setChecked( m_rawKeyboard );
        ui.chkVelocityColor->setChecked( m_velocityColor );
        ui.chkEnforceChannelState->setChecked( m_enforceChannelState );
        ui.chkEnableKeyboard->setChecked( m_enableKeyboard );
        ui.chkEnableMouse->setChecked( m_enableMouse );
        ui.chkEnableTouch->setChecked( m_enableTouch );
        ui.cboColorPolicy->setCurrentIndex( m_colorDialog->currentPalette()->paletteId() );
        ui.cboStartingKey->setCurrentIndex( ui.cboStartingKey->findData(m_startingKey));
    }
}

void Preferences::apply()
{
    m_numKeys = ui.spinNumKeys->value();
    m_alwaysOnTop = ui.chkAlwaysOnTop->isChecked();
    m_rawKeyboard = ui.chkRawKeyboard->isChecked();
    m_velocityColor = ui.chkVelocityColor->isChecked();
    m_enforceChannelState = ui.chkEnforceChannelState->isChecked();
    m_enableKeyboard = ui.chkEnableKeyboard->isChecked();
    m_enableMouse = ui.chkEnableMouse->isChecked();
    m_enableTouch = ui.chkEnableTouch->isChecked();
    if ( ui.txtFileRawKmap->text().isEmpty() ||
         ui.txtFileRawKmap->text() == QSTR_DEFAULT)
        m_rawmap.setFileName(QSTR_DEFAULT);
    if ( ui.txtFileKmap->text().isEmpty() ||
         ui.txtFileKmap->text() == QSTR_DEFAULT)
        m_keymap.setFileName(QSTR_DEFAULT);
    if ( ui.txtFileInstrument->text().isEmpty() ||
         ui.txtFileInstrument->text() == QSTR_DEFAULT )
        m_insFileName = QSTR_DEFAULT;
    m_drumsChannel = ui.cboDrumsChannel->currentIndex() - 1;
    m_colorDialog->loadPalette(ui.cboColorPolicy->currentIndex());
    m_startingKey = ui.cboStartingKey->itemData(ui.cboStartingKey->currentIndex()).toInt();
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
    if (m_colorDialog != 0) {
        m_colorDialog->loadPalette(ui.cboColorPolicy->currentIndex());
        if(m_colorDialog->exec() == QDialog::Accepted) {
            ui.cboColorPolicy->setCurrentIndex(m_colorDialog->currentPalette()->paletteId());
            m_colorDialog->saveCurrentPalette();
        }
    }
}

Instrument* Preferences::getInstrument()
{
    QString key = ui.cboInstrument->currentText();
    if (key.isEmpty())
        return NULL;
    if (!m_ins.contains(key))
        return NULL;
    return &m_ins[key];
}

Instrument* Preferences::getDrumsInstrument()
{
    QString key = ui.cboInstrument->currentText();
    if (key.isEmpty())
        return NULL;
    key.append(" Drums");
    if (!m_ins.contains(key))
        return NULL;
    return &m_ins[key];
}

void Preferences::setInstrumentsFileName( const QString fileName )
{
    QFileInfo f(fileName);
    if (f.isReadable()) {
        m_ins.clear();
        ui.cboInstrument->clear();
        if (m_ins.load(fileName)) {
            ui.txtFileInstrument->setText(f.fileName());
            InstrumentList::ConstIterator it;
            for(it = m_ins.constBegin(); it != m_ins.constEnd(); ++it) {
                if(!it.key().endsWith(QLatin1String("Drums"), Qt::CaseInsensitive))
                    ui.cboInstrument->addItem(it.key());
            }
            ui.cboInstrument->setCurrentIndex(-1);
            m_insFileName = fileName;
        } else {
            m_insFileName.clear();
            ui.txtFileInstrument->setText(m_insFileName);
        }
    } else {
        qDebug() << "file" << fileName << "not readable.";
    }
}

QString Preferences::getInstrumentsFileName()
{
    return m_insFileName;
}

void Preferences::setInstrumentName( const QString name )
{
    int index = ui.cboInstrument->findText( name );
    ui.cboInstrument->setCurrentIndex( index );
}

QString Preferences::getInstrumentName()
{
    return ui.cboInstrument->currentText();
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

void Preferences::setRawKeyMapFileName( const QString fileName )
{
    QFileInfo f(fileName);
    if (f.isReadable()) {
        m_rawmap.loadFromXMLFile(fileName);
        ui.txtFileRawKmap->setText(f.fileName());
    } else {
        m_rawmap.clear();
        m_rawmap.setFileName(QSTR_DEFAULT);
    }
}

void Preferences::setKeyMapFileName( const QString fileName )
{
    QFileInfo f(fileName);
    if (f.isReadable()) {
        m_keymap.loadFromXMLFile(fileName);
        ui.txtFileKmap->setText(f.fileName());
    } else {
        m_keymap.clear();
        m_keymap.setFileName(QSTR_DEFAULT);
    }
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
}

void Preferences::slotRestoreDefaults()
{
    restoreDefaults();
}

void Preferences::retranslateUi()
{
    ui.retranslateUi(this);
    if (m_colorDialog != 0) {
        ui.cboColorPolicy->clear();
        ui.cboColorPolicy->addItems(m_colorDialog->availablePaletteNames());
    }
}

void Preferences::setColorPolicyDialog(ColorDialog *value)
{
    if (value != 0) {
        m_colorDialog = value;
        ui.cboColorPolicy->addItems(m_colorDialog->availablePaletteNames());
    }
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
}
