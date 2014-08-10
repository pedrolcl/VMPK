/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2014, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#ifndef EXTRACONTROLS_H
#define EXTRACONTROLS_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
    class DialogExtraControls;
}

const QListWidgetItem::ItemType extraControlType = QListWidgetItem::ItemType(QListWidgetItem::UserType + 1);

class ExtraControl : public QListWidgetItem {
public:
    ExtraControl( QListWidget *parent = 0, int type = extraControlType ):
            QListWidgetItem( parent, type ),
            m_type(0), m_minValue(0), m_maxValue(127), m_defValue(0), m_size(100) {}
    virtual ~ExtraControl() {}
    void setControl(int ctl) { m_control = ctl; }
    void setType(int type) { m_type = type; }
    void setMinimum(int v) { m_minValue = v; }
    void setMaximum(int v) { m_maxValue = v; }
    void setDefault(int v) { m_defValue = v; }
    void setSize(int s) { m_size = s; }
    void setOnValue(int v) { m_maxValue = v; }
    void setOffValue(int v) { m_minValue = v; }
    void setOnDefault(bool b) { m_defValue = int(b); }
    void setFileName(QString s) { m_fileName = s; }
    int getControl() { return m_control; }
    int getType() { return m_type; }
    int getMinimum() { return m_minValue; }
    int getMaximum() { return m_maxValue; }
    int getDefault() { return m_defValue; }
    int getSize() { return m_size; }
    int getOnValue() { return m_maxValue; }
    int getOffValue() { return m_minValue; }
    bool getOnDefault() { return bool(m_defValue); }
    QString getFileName() { return m_fileName; }

    QString toString();
    void initFromString(const QString s);

    static int mbrFromString( const QString s, int def );
    static void decodeString( const QString s, QString& label,
                              int& control,
                              int& type,
                              int& minValue,
                              int& maxValue,
                              int& defValue,
                              int& size,
                              QString& fileName);

private:
    int m_control;
    int m_type;
    int m_minValue;
    int m_maxValue;
    int m_defValue;
    int m_size;
    QString m_fileName;
};

class DialogExtraControls : public QDialog {
    Q_OBJECT
public:
    DialogExtraControls(QWidget *parent = 0);
    ~DialogExtraControls();
    void setControls(const QStringList& ctls);
    QStringList getControls();
    void retranslateUi();

public slots:
    void addControl();
    void removeControl();
    void controlUp();
    void controlDown();
    void itemSelected(QListWidgetItem *current, QListWidgetItem *previous);
    void labelEdited(QString newlabel);
    void controlChanged(int control);
    void typeChanged(int type);
    void minimumChanged(int minimum);
    void maximumChanged(int maximum);
    void onvalueChanged(int onvalue);
    void offvalueChanged(int offvalue);
    void defaultChanged(int defvalue);
    void defOnChanged(bool defOn);
    void sizeChanged(int size);
    void openFile();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DialogExtraControls *m_ui;
};

#endif // EXTRACONTROLS_H
