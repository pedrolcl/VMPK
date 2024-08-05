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

#ifndef SHORTCUTDIALOG_H
#define SHORTCUTDIALOG_H

#include "ui_shortcutdialog.h"

#include <QList>
#include <QItemDelegate>
#include <QLineEdit>


class QAction;
class QToolButton;

class ShortcutTableItemEdit;

//-------------------------------------------------------------------------
// ShortcutTableItemEditor

class ShortcutTableItemEditor : public QWidget
{
	Q_OBJECT

public:

	// Constructor.
	ShortcutTableItemEditor(QWidget *pParent = nullptr);

	// Shortcut text accessors.
	void setText(const QString& sText);
	QString text() const;

	// Default (initial) shortcut text accessors.
	void setDefaultText(const QString& sDefaultText)
		{ m_sDefaultText = sDefaultText; }
	const QString& defaultText() const
		{ return m_sDefaultText; }

signals:
	void editingFinished();
    void valueChanged(QString text);

protected slots:

	void clear();
	void finish();

private:

	// Instance variables.
	ShortcutTableItemEdit *m_pLineEdit;
	QToolButton *m_pToolButton;
	QString m_sDefaultText;
};


//-------------------------------------------------------------------------
// ShortcutTableItemDelegate

class ShortcutTableItemDelegate : public QItemDelegate
{
	Q_OBJECT

public:

	// Constructor.
	ShortcutTableItemDelegate(QTableWidget *pTableWidget);

protected:

	void paint(QPainter *pPainter,
		const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

	QWidget *createEditor(QWidget *pParent, 
		const QStyleOptionViewItem& option,
        const QModelIndex & index) const override;

	void setEditorData(QWidget *pEditor,
        const QModelIndex &index) const override;
	void setModelData(QWidget *pEditor,
		QAbstractItemModel *pModel,
        const QModelIndex& index) const override;

protected slots:

	void commitEditor();

private:

	QTableWidget *m_pTableWidget;
};


//-------------------------------------------------------------------------
// ShortcutDialog

class ShortcutDialog : public QDialog
{
	Q_OBJECT

public:

	// Constructor.
	ShortcutDialog(QList<QAction *> actions, QWidget *pParent = nullptr);
    void retranslateUi();
    void setDefaultShortcuts(QHash<QString,QList<QKeySequence> > &defs);

protected slots:

	void actionActivated(QTableWidgetItem *);
	void actionChanged(QTableWidgetItem *);
    void slotRestoreDefaults();

    void accept() override;
    void reject() override;

private:

	// The Qt-designer UI struct...
	Ui::ShortcutDialog m_ui;

	QList<QAction *> m_actions;
    QHash<QString,QList<QKeySequence> > m_defaultShortcuts;
    int m_iDirtyCount;
};


#endif	// SHORTCUTDIALOG_H

// end of ShortcutDialog.h
