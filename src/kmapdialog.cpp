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

#include <QFileInfo>
#include <QHeaderView>
#include <QPushButton>
#include <QKeySequence>
#include <QFileDialog>
#include <drumstick/pianokeybd.h>
#include "kmapdialog.h"
#include "vpianosettings.h"

KMapDialog::KMapDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	m_btnOpen = ui.buttonBox->addButton(tr("Open..."), QDialogButtonBox::ActionRole);
    m_btnSave = ui.buttonBox->addButton(tr("Save As..."), QDialogButtonBox::ActionRole);
    m_btnOpen->setIcon(style()->standardIcon(QStyle::StandardPixmap(QStyle::SP_DialogOpenButton)));
    m_btnSave->setIcon(style()->standardIcon(QStyle::StandardPixmap(QStyle::SP_DialogSaveButton)));
    connect(m_btnOpen, &QPushButton::clicked, this, &KMapDialog::slotOpen);
    connect(m_btnSave, &QPushButton::clicked, this, &KMapDialog::slotSave);
}

void KMapDialog::displayMap(const VMPKKeyboardMap* map)
{
    int row;
    if (map != nullptr) m_map.copyFrom(map);
    setWindowTitle(m_map.getRawMode() ? tr("Raw Key Map Editor") : tr("Key Map Editor"));
    ui.tableWidget->clearContents();
    ui.tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(m_map.getRawMode() ? tr("Key Code") : tr("Key")));
    QFileInfo f(m_map.getFileName());
    ui.labelMapName->setText(f.fileName());
    KeyboardMap::ConstIterator it;
    for(it = m_map.constBegin(); it != m_map.constEnd(); ++it) {
        row = it.value();
        if (m_map.getRawMode()) {
            ui.tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(it.key())));
        } else {
            QKeySequence ks(it.key());
            ui.tableWidget->setItem(row, 0, new QTableWidgetItem(ks.toString()));
        }
    }
}

void KMapDialog::updateMap()
{
    bool ok;
    m_map.clear();
    QTableWidgetItem* item;
    for( int i=0; i<128; ++i) {
        item = ui.tableWidget->item(i, 0);
        if ((item != nullptr) && !item->text().isEmpty()) {
            if (m_map.getRawMode()) {
                int keycode = item->text().toInt(&ok);
                if (ok) m_map.insert(keycode, i);
            } else {
                QKeySequence ks(item->text());
			#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
                m_map.insert(ks[0], i);
			#else
				m_map.insert(ks[0].key(), i);
			#endif
            }
        }
    }
}

void KMapDialog::getMap(VMPKKeyboardMap* map)
{
    updateMap();
    map->copyFrom(&m_map);
}

void KMapDialog::slotOpen()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,
                                tr("Open keyboard map definition"),
                                VPianoSettings::dataDirectory(),
                                tr("Keyboard map (*.xml)"));
    if (!fileName.isEmpty()) {
        m_map.clear();
        m_map.loadFromXMLFile(fileName);
        displayMap(nullptr);
    }
}

void KMapDialog::slotSave()
{
    QFileDialog dlg(this);
    dlg.setNameFilter(tr("Keyboard map (*.xml)"));
    dlg.setDirectory(VPianoSettings::dataDirectory());
    dlg.setWindowTitle(tr("Save keyboard map definition"));
    dlg.setDefaultSuffix("xml");
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    if (dlg.exec() == QDialog::Accepted) {
        QString fileName = dlg.selectedFiles().first();
        updateMap();
        m_map.saveToXMLFile(fileName);
    }
}

void KMapDialog::retranslateUi()
{
    ui.retranslateUi(this);
    m_btnOpen->setText(tr("Open..."));
    m_btnSave->setText(tr("Save As..."));
}
