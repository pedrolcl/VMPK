/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2024, Pedro Lopez-Cabanillas <plcl@users.sf.net>
    Copyright (C) 2005-2024, rncbc aka Rui Nuno Capela. All rights reserved.

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

#include "shortcutdialog.h"

#include <QAction>
#include <QMessageBox>
#include <QPushButton>
#include <QHeaderView>

#include <QPainter>
#include <QLineEdit>
#include <QToolButton>
#include <QKeyEvent>

//-------------------------------------------------------------------------
// ShortcutTableItem

class ShortcutTableItem : public QTableWidgetItem
{
public:

	// Constructors.
	ShortcutTableItem(const QIcon& icon, const QString& sText)
		: QTableWidgetItem(icon, sText)
		{ setFlags(flags() & ~Qt::ItemIsEditable); }
	ShortcutTableItem(const QString& sText)
		: QTableWidgetItem(sText)
		{ setFlags(flags() & ~Qt::ItemIsEditable); }
	ShortcutTableItem(const QKeySequence& shortcut)
        : QTableWidgetItem(shortcut.toString()) {}
};


//-------------------------------------------------------------------------
// ShortcutTableItemEdit

class ShortcutTableItemEdit : public QLineEdit
{
public:

	// Constructor.
	ShortcutTableItemEdit(QWidget *pParent = nullptr)
		: QLineEdit(pParent) {}

protected:

	// Shortcut key to text event translation.
    void keyPressEvent(QKeyEvent *pKeyEvent) override
	{
		int iKey = pKeyEvent->key();
		Qt::KeyboardModifiers modifiers = pKeyEvent->modifiers();

		if (iKey == Qt::Key_Return && modifiers == Qt::NoModifier) {
			emit editingFinished();
			return;
		}
	
		if (iKey >= Qt::Key_Shift && iKey < Qt::Key_F1) {
			QLineEdit::keyPressEvent(pKeyEvent);
			return;
		}

		if (modifiers & Qt::ShiftModifier)
			iKey |= Qt::SHIFT;
		if (modifiers & Qt::ControlModifier)
			iKey |= Qt::CTRL;
		if (modifiers & Qt::AltModifier)
			iKey |= Qt::ALT;

        QLineEdit::setText(QKeySequence(iKey).toString());
	}
};


//-------------------------------------------------------------------------
// ShortcutTableItemEditor

ShortcutTableItemEditor::ShortcutTableItemEditor (
	QWidget *pParent ) : QWidget(pParent)
{
	m_pLineEdit = new ShortcutTableItemEdit(/*this*/);

	m_pToolButton = new QToolButton(/*this*/);
	m_pToolButton->setFixedWidth(18);
	m_pToolButton->setText("X");

	QHBoxLayout *pLayout = new QHBoxLayout();
	pLayout->setSpacing(0);
    pLayout->setContentsMargins(0,0,0,0);
	pLayout->addWidget(m_pLineEdit);
	pLayout->addWidget(m_pToolButton);
	QWidget::setLayout(pLayout);
	
	QWidget::setFocusPolicy(Qt::StrongFocus);
	QWidget::setFocusProxy(m_pLineEdit);

    connect(m_pLineEdit,
            &ShortcutTableItemEdit::editingFinished,
            this,
            &ShortcutTableItemEditor::finish);
    connect(m_pToolButton, &QToolButton::clicked, this, &ShortcutTableItemEditor::clear);
}


// Shortcut text accessors.
void ShortcutTableItemEditor::setText ( const QString& sText )
{
	m_pLineEdit->setText(sText);
}


QString ShortcutTableItemEditor::text (void) const
{
	return m_pLineEdit->text();
}


// Shortcut text clear/toggler.
void ShortcutTableItemEditor::clear (void)
{
	if (m_pLineEdit->text() == m_sDefaultText)
		m_pLineEdit->clear();
	else
		m_pLineEdit->setText(m_sDefaultText);

	m_pLineEdit->setFocus();
}


// Shortcut text finish notification.
void ShortcutTableItemEditor::finish (void)
{
	emit editingFinished();
    emit valueChanged(m_pLineEdit->text());
}


//-------------------------------------------------------------------------
// ShortcutTableItemDelegate

ShortcutTableItemDelegate::ShortcutTableItemDelegate (
	QTableWidget *pTableWidget )
	: QItemDelegate(pTableWidget), m_pTableWidget(pTableWidget)
{
}


// Overridden paint method.
void ShortcutTableItemDelegate::paint ( QPainter *pPainter,
	const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
	// Special treatment for action icon+text...
	if (index.column() == 0) {
		QTableWidgetItem *pItem = m_pTableWidget->item(index.row(), 0);
		pPainter->save();
		if (option.state & QStyle::State_Selected) {
			const QPalette& pal = option.palette;
			pPainter->fillRect(option.rect, pal.highlight().color());
			pPainter->setPen(pal.highlightedText().color());
		}
		// Draw the icon...
		QRect rect = option.rect;
		const QSize& iconSize = m_pTableWidget->iconSize();
		pPainter->drawPixmap(1,
			rect.top() + ((rect.height() - iconSize.height()) >> 1),
			pItem->icon().pixmap(iconSize));
		// Draw the text...
		rect.setLeft(iconSize.width() + 2);
		pPainter->drawText(rect,
			Qt::TextShowMnemonic | Qt::AlignLeft | Qt::AlignVCenter,
			pItem->text());
		pPainter->restore();
	} else {
		// Others do as default...
		QItemDelegate::paint(pPainter, option, index);
	}
}


QWidget *ShortcutTableItemDelegate::createEditor ( QWidget *pParent,
	const QStyleOptionViewItem& /*option*/, const QModelIndex& index ) const
{
	ShortcutTableItemEditor *pItemEditor
		= new ShortcutTableItemEditor(pParent);
	pItemEditor->setDefaultText(
		index.model()->data(index, Qt::DisplayRole).toString());
    connect(pItemEditor,
            &ShortcutTableItemEditor::editingFinished,
            this,
            &ShortcutTableItemDelegate::commitEditor);
    return pItemEditor;
}


void ShortcutTableItemDelegate::setEditorData ( QWidget *pEditor,
	const QModelIndex& index ) const
{
	ShortcutTableItemEditor *pItemEditor
		= qobject_cast<ShortcutTableItemEditor *> (pEditor);
	pItemEditor->setText(
		index.model()->data(index, Qt::DisplayRole).toString());
}


void ShortcutTableItemDelegate::setModelData ( QWidget *pEditor,
	QAbstractItemModel *pModel, const QModelIndex& index ) const
{
	ShortcutTableItemEditor *pItemEditor
		= qobject_cast<ShortcutTableItemEditor *> (pEditor);
	pModel->setData(index, pItemEditor->text());
}


void ShortcutTableItemDelegate::commitEditor (void)
{
	ShortcutTableItemEditor *pItemEditor
		= qobject_cast<ShortcutTableItemEditor *> (sender());
	emit commitData(pItemEditor);
	emit closeEditor(pItemEditor);
}


//-------------------------------------------------------------------------
// ShortcutDialog

ShortcutDialog::ShortcutDialog ( QList<QAction *> actions,
	QWidget *pParent ) : QDialog(pParent)
{
	// Setup UI struct...
	m_ui.setupUi(this);

	// Window modality (let plugin/tool windows rave around).
	QDialog::setWindowModality(Qt::ApplicationModal);

	m_iDirtyCount = 0;
	m_ui.ShortcutTable->setIconSize(QSize(16, 16));
	m_ui.ShortcutTable->setItemDelegate(
		new ShortcutTableItemDelegate(m_ui.ShortcutTable));
	m_ui.ShortcutTable->horizontalHeader()->setStretchLastSection(true);
	m_ui.ShortcutTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
	m_ui.ShortcutTable->horizontalHeader()->resizeSection(0, 120);
	m_ui.ShortcutTable->horizontalHeader()->resizeSection(1, 260);
	int iRowHeight = m_ui.ShortcutTable->fontMetrics().height() + 4;
	m_ui.ShortcutTable->verticalHeader()->setDefaultSectionSize(iRowHeight);
	m_ui.ShortcutTable->verticalHeader()->hide();

	int iRow = 0;
	QListIterator<QAction *> iter(actions);
	while (iter.hasNext()) {
		QAction *pAction = iter.next();
		if (pAction->objectName().isEmpty())
			continue;
		m_ui.ShortcutTable->insertRow(iRow);
		m_ui.ShortcutTable->setItem(iRow, 0,
			new ShortcutTableItem(pAction->icon(), pAction->text()));
		m_ui.ShortcutTable->setItem(iRow, 1,
			new ShortcutTableItem(pAction->statusTip()));
		m_ui.ShortcutTable->setItem(iRow, 2,
			new ShortcutTableItem(pAction->shortcut()));
		m_actions.append(pAction);
		++iRow;
	}

    connect(m_ui.ShortcutTable,
            &QTableWidget::itemActivated,
            this,
            &ShortcutDialog::actionActivated);

    connect(m_ui.ShortcutTable, &QTableWidget::itemChanged, this, &ShortcutDialog::actionChanged);

    connect(m_ui.DialogButtonBox, &QDialogButtonBox::accepted, this, &ShortcutDialog::accept);
    connect(m_ui.DialogButtonBox, &QDialogButtonBox::rejected, this, &ShortcutDialog::reject);

    QPushButton *btnDefaults = m_ui.DialogButtonBox->button(QDialogButtonBox::RestoreDefaults);
    connect(btnDefaults, &QPushButton::clicked, this, &ShortcutDialog::slotRestoreDefaults);
}


void ShortcutDialog::actionActivated ( QTableWidgetItem *pItem )
{
	m_ui.ShortcutTable->editItem(m_ui.ShortcutTable->item(pItem->row(), 2));
}


void ShortcutDialog::actionChanged ( QTableWidgetItem *pItem )
{
    pItem->setText(QKeySequence(pItem->text().trimmed()).toString());
	m_iDirtyCount++;
}


void ShortcutDialog::accept (void)
{
	if (m_iDirtyCount > 0) {
		for (int iRow = 0; iRow < m_actions.count(); ++iRow) {
			QAction *pAction = m_actions[iRow];
			pAction->setShortcut(
				QKeySequence(m_ui.ShortcutTable->item(iRow, 2)->text()));
		}
	}

	QDialog::accept();
}


void ShortcutDialog::reject (void)
{
	bool bReject = true;

	// Check if there's any pending changes...
	if (m_iDirtyCount > 0) {
		QMessageBox::StandardButtons buttons
			= QMessageBox::Discard | QMessageBox::Cancel;
		if (m_ui.DialogButtonBox->button(QDialogButtonBox::Ok)->isEnabled())
			buttons |= QMessageBox::Apply;
		switch (QMessageBox::warning(this,
			tr("Warning"),
			tr("Keyboard shortcuts have been changed.\n\n"
			"Do you want to apply the changes?"),
			buttons)) {
		case QMessageBox::Apply:
			accept();
			return;
		case QMessageBox::Discard:
			break;
		default:    // Cancel.
			bReject = false;
		}
	}

	if (bReject)
		QDialog::reject();
}

void ShortcutDialog::retranslateUi()
{
    m_ui.retranslateUi(this);
}

void ShortcutDialog::slotRestoreDefaults()
{
    for (int iRow = 0; iRow < m_actions.count(); ++iRow) {
        QAction *pAction = m_actions[iRow];
        QString sKey = '/' + pAction->objectName();
        pAction->setShortcuts(m_defaultShortcuts[sKey]);
        m_ui.ShortcutTable->item(iRow, 2)->setText(pAction->shortcut().toString());
    }
}

void ShortcutDialog::setDefaultShortcuts(QHash<QString,QList<QKeySequence> > &defs)
{
    m_defaultShortcuts = defs;
}

// end of ShortcutDialog.cpp
