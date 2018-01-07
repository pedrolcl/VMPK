/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2018, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include "extracontrols.h"
#include "ui_extracontrols.h"
#include <QFileDialog>
#include <QPushButton>

DialogExtraControls::DialogExtraControls(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DialogExtraControls)
{
    m_ui->setupUi(this);
    m_ui->btnUp->setIcon(style()->standardIcon(QStyle::StandardPixmap(QStyle::SP_ArrowUp)));
    m_ui->btnDown->setIcon(style()->standardIcon(QStyle::StandardPixmap(QStyle::SP_ArrowDown)));
    m_ui->btnAdd->setIcon(QIcon::fromTheme("list-add", QIcon(":/vpiano/list-add.svg")));
    m_ui->btnRemove->setIcon(QIcon::fromTheme("list-remove", QIcon(":/vpiano/list-remove.svg")));
    connect( m_ui->btnAdd, SIGNAL(clicked()), SLOT(addControl()) );
    connect( m_ui->btnRemove, SIGNAL(clicked()), SLOT(removeControl()) );
    connect( m_ui->btnUp, SIGNAL(clicked()), SLOT(controlUp()) );
    connect( m_ui->btnDown, SIGNAL(clicked()), SLOT(controlDown()) );
    connect( m_ui->extraList,
             SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
             SLOT(itemSelected(QListWidgetItem *, QListWidgetItem*)) );
    connect( m_ui->txtLabel, SIGNAL(textEdited(QString)), SLOT(labelEdited(QString)) );
    connect( m_ui->spinController, SIGNAL(valueChanged(int)), SLOT(controlChanged(int)) );
    connect( m_ui->cboControlType, SIGNAL(currentIndexChanged(int)), SLOT(typeChanged(int)) );
    connect( m_ui->chkSwitchDefOn, SIGNAL(toggled(bool)), SLOT(defOnChanged(bool)) );
    connect( m_ui->spinKnobDef, SIGNAL(valueChanged(int)), SLOT(defaultChanged(int)) );
    connect( m_ui->spinSpinDef, SIGNAL(valueChanged(int)), SLOT(defaultChanged(int)) );
    connect( m_ui->spinSliderDef, SIGNAL(valueChanged(int)), SLOT(defaultChanged(int)) );
    connect( m_ui->spinKnobMax, SIGNAL(valueChanged(int)), SLOT(maximumChanged(int)) );
    connect( m_ui->spinSpinMax, SIGNAL(valueChanged(int)), SLOT(maximumChanged(int)) );
    connect( m_ui->spinSliderMax, SIGNAL(valueChanged(int)), SLOT(maximumChanged(int)) );
    connect( m_ui->spinKnobMin, SIGNAL(valueChanged(int)), SLOT(minimumChanged(int)) );
    connect( m_ui->spinSpinMin, SIGNAL(valueChanged(int)), SLOT(minimumChanged(int)) );
    connect( m_ui->spinSliderMin, SIGNAL(valueChanged(int)), SLOT(minimumChanged(int)) );
    connect( m_ui->spinValueOff, SIGNAL(valueChanged(int)), SLOT(minimumChanged(int)) );
    connect( m_ui->spinValueOn, SIGNAL(valueChanged(int)), SLOT(maximumChanged(int)) );
    connect( m_ui->spinSliderSize, SIGNAL(valueChanged(int)), SLOT(sizeChanged(int)) );
    connect( m_ui->spinValue, SIGNAL(valueChanged(int)), SLOT(minimumChanged(int)) );
    connect( m_ui->keySeqBtnCtl, SIGNAL(valueChanged(QString)), SLOT(shortcutChanged(QString)) );
    connect( m_ui->keySeqBtnSyx, SIGNAL(valueChanged(QString)), SLOT(shortcutChanged(QString)) );
    connect( m_ui->keySeqSwitch, SIGNAL(valueChanged(QString)), SLOT(shortcutChanged(QString)) );
    connect( m_ui->btnFileSyx, SIGNAL(clicked()), SLOT(openFile()) );
#if defined(SMALL_SCREEN)
    setWindowState(Qt::WindowActive | Qt::WindowMaximized);
#endif
}

DialogExtraControls::~DialogExtraControls()
{
    delete m_ui;
}

void DialogExtraControls::addControl()
{
    ExtraControl *e = new ExtraControl(m_ui->extraList);
    e->setText(tr("New Control"));
    m_ui->extraList->setCurrentItem(e);
}

void DialogExtraControls::removeControl()
{
    int row = m_ui->extraList->currentRow();
    QListWidgetItem *e = m_ui->extraList->takeItem(row);
    delete e;
}

void DialogExtraControls::controlUp()
{
    int row = m_ui->extraList->currentRow();
    QListWidgetItem *e = m_ui->extraList->takeItem(row);
    if (e != NULL) {
        m_ui->extraList->insertItem(row - 1, e);
        m_ui->extraList->setCurrentItem(e);
    }
}

void DialogExtraControls::controlDown()
{
    int row = m_ui->extraList->currentRow();
    QListWidgetItem *e = m_ui->extraList->takeItem(row);
    if (e != NULL) {
        m_ui->extraList->insertItem(row + 1, e);
        m_ui->extraList->setCurrentItem(e);
    }
}

void DialogExtraControls::itemSelected( QListWidgetItem *current, QListWidgetItem * )
{
    ExtraControl *e = dynamic_cast<ExtraControl*>(current);
    m_ui->commonFrame->setEnabled( e != NULL );
    m_ui->cboControlType->setEnabled( e != NULL );
    m_ui->stackedPanel->setEnabled( e != NULL );
    m_ui->btnRemove->setEnabled(e != NULL );
    m_ui->btnUp->setEnabled(e != NULL && m_ui->extraList->currentRow() > 0);
    m_ui->btnDown->setEnabled(e != NULL && m_ui->extraList->currentRow() < (m_ui->extraList->count()-1));
    if (e != NULL) {
        m_ui->txtLabel->setText(e->text());
        m_ui->spinController->setValue(e->getControl());
        m_ui->cboControlType->setCurrentIndex(e->getType());
        m_ui->spinKnobDef->setValue(e->getDefault());
        m_ui->spinSpinDef->setValue(e->getDefault());
        m_ui->spinSliderDef->setValue(e->getDefault());
        m_ui->spinKnobMin->setValue(e->getMinimum());
        m_ui->spinSpinMin->setValue(e->getMinimum());
        m_ui->spinSliderMin->setValue(e->getMinimum());
        m_ui->spinKnobMax->setValue(e->getMaximum());
        m_ui->spinSpinMax->setValue(e->getMaximum());
        m_ui->spinSliderMax->setValue(e->getMaximum());
        m_ui->spinValueOff->setValue(e->getOffValue());
        m_ui->spinValueOn->setValue(e->getOnValue());
        m_ui->chkSwitchDefOn->setChecked(e->getOnDefault());
        m_ui->spinSliderSize->setValue(e->getSize());
        m_ui->spinValue->setValue(e->getMinimum());
        m_ui->edtFileSyx->setText(e->getFileName());
        m_ui->keySeqBtnCtl->setText(e->getShortcut());
        m_ui->keySeqSwitch->setText(e->getShortcut());
        m_ui->keySeqBtnSyx->setText(e->getShortcut());
    }
}

void DialogExtraControls::labelEdited(QString newLabel)
{
    ExtraControl *e = dynamic_cast<ExtraControl*>(m_ui->extraList->currentItem());
    if (e != NULL) e->setText(newLabel);
}

void DialogExtraControls::controlChanged(int control)
{
    ExtraControl *e = dynamic_cast<ExtraControl*>(m_ui->extraList->currentItem());
    if (e != NULL) e->setControl(control);
}

void DialogExtraControls::typeChanged(int type)
{
    ExtraControl *e = dynamic_cast<ExtraControl*>(m_ui->extraList->currentItem());
    if (e != NULL)  {
        e->setType(type);
        if (type == ExtraControl::ControlType::ButtonSyxControl) {
            e->setControl(255);
            m_ui->spinController->setEnabled(false);
        } else
            m_ui->spinController->setEnabled(true);
    }
}

void DialogExtraControls::minimumChanged(int minimum)
{
    ExtraControl *e = dynamic_cast<ExtraControl*>(m_ui->extraList->currentItem());
    if (e != NULL) e->setMinimum(minimum);
}

void DialogExtraControls::maximumChanged(int maximum)
{
    ExtraControl *e = dynamic_cast<ExtraControl*>(m_ui->extraList->currentItem());
    if (e != NULL) e->setMaximum(maximum);
}

void DialogExtraControls::onvalueChanged(int onvalue)
{
    ExtraControl *e = dynamic_cast<ExtraControl*>(m_ui->extraList->currentItem());
    if (e != NULL) e->setOnValue(onvalue);
}

void DialogExtraControls::offvalueChanged(int offvalue)
{
    ExtraControl *e = dynamic_cast<ExtraControl*>(m_ui->extraList->currentItem());
    if (e != NULL) e->setOffValue(offvalue);
}

void DialogExtraControls::defaultChanged(int defvalue)
{
    ExtraControl *e = dynamic_cast<ExtraControl*>(m_ui->extraList->currentItem());
    if (e != NULL) e->setDefault(defvalue);
}

void DialogExtraControls::defOnChanged(bool defOn)
{
    ExtraControl *e = dynamic_cast<ExtraControl*>(m_ui->extraList->currentItem());
    if (e != NULL) e->setOnDefault(defOn);
}

void DialogExtraControls::sizeChanged(int size)
{
    ExtraControl *e = dynamic_cast<ExtraControl*>(m_ui->extraList->currentItem());
    if (e != NULL) e->setSize(size);
}

void DialogExtraControls::shortcutChanged(QString keySequence)
{
    ExtraControl *e = dynamic_cast<ExtraControl*>(m_ui->extraList->currentItem());
    if (e != NULL) e->setShortcut(keySequence);
}

void DialogExtraControls::setControls(const QStringList& ctls)
{
    m_ui->extraList->clear();
    foreach(const QString& s, ctls) {
        ExtraControl *item = new ExtraControl(m_ui->extraList);
        item->initFromString(s);
    }
}

QStringList DialogExtraControls::getControls()
{
    QStringList lst;
    for(int row = 0; row < m_ui->extraList->count(); ++row) {
        ExtraControl *item = static_cast<ExtraControl *>(m_ui->extraList->item(row));
        lst << item->toString();
    }
    return lst;
}

void DialogExtraControls::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void DialogExtraControls::openFile()
{
    ExtraControl *e = dynamic_cast<ExtraControl*>(m_ui->extraList->currentItem());
    if (e != NULL) {
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("System Exclusive File"), QString::null, tr("System Exclusive (*.syx)"));
        if (!fileName.isEmpty()) {
            m_ui->edtFileSyx->setText(fileName);
            e->setFileName(fileName);
        }
    }
}

void DialogExtraControls::retranslateUi()
{
    m_ui->retranslateUi(this);
}

QString ExtraControl::toString()
{
    QStringList lst;
    lst << text();
    lst << QString::number(m_control);
    lst << QString::number(m_type);
    lst << QString::number(m_minValue);
    lst << QString::number(m_maxValue);
    lst << QString::number(m_defValue);
    if (m_type == ControlType::SliderControl)
        lst << QString::number(m_size);
    if (m_type == ControlType::ButtonSyxControl)
        lst << m_fileName;
    if (m_type == ControlType::SwitchControl ||
        m_type == ControlType::ButtonCtlControl ||
        m_type == ControlType::ButtonSyxControl)
        lst << m_keySequence;
    return lst.join(",");
}

int ExtraControl::mbrFromString(const QString sTmp, int def)
{
    bool ok;
    int iTmp = sTmp.toInt(&ok);
    if (ok) return iTmp;
    return def;
}

void ExtraControl::decodeString( const QString s,
                          QString& label,
                          int& control,
                          int& type,
                          int& minValue,
                          int& maxValue,
                          int& defValue,
                          int& size,
                          QString& fileName,
                          QString& shortcutKey)
{
    QStringList lst = s.split(",");
    if (!lst.isEmpty())
        label = lst.takeFirst();
    if (!lst.isEmpty())
        control = ExtraControl::mbrFromString(lst.takeFirst(), 0);
    if (!lst.isEmpty())
        type = ExtraControl::mbrFromString(lst.takeFirst(), 0);
    if (!lst.isEmpty())
        minValue = ExtraControl::mbrFromString(lst.takeFirst(), 0);
    if (!lst.isEmpty())
        maxValue = ExtraControl::mbrFromString(lst.takeFirst(), 127);
    if (!lst.isEmpty())
        defValue = ExtraControl::mbrFromString(lst.takeFirst(), 0);
    if (!lst.isEmpty() && type == ControlType::SliderControl)
        size = ExtraControl::mbrFromString(lst.takeFirst(), 100);
    if (!lst.isEmpty() && type == ControlType::ButtonSyxControl)
        fileName = lst.takeFirst();
    if (!lst.isEmpty() && (type == ControlType::SwitchControl ||
                           type == ControlType::ButtonCtlControl ||
                           type == ControlType::ButtonSyxControl))
        shortcutKey = lst.takeFirst();
}

void ExtraControl::initFromString(const QString s)
{
    QString lbl;
    ExtraControl::decodeString( s, lbl, m_control, m_type,
                                m_minValue, m_maxValue, m_defValue,
                                m_size, m_fileName, m_keySequence );
    setText(lbl);
}
