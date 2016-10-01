/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2016, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <QSettings>
#include <QNetworkInterface>
#include <QDialogButtonBox>
#include <QPushButton>
#include "networksettingsdialog.h"
#include "ui_networksettingsdialog.h"

const QString QSTR_ADDRESS(QLatin1Literal("225.0.0.37"));

NetworkSettingsDialog::NetworkSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkSettingsDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::pressed,
            this, &NetworkSettingsDialog::restoreDefaults);
}

NetworkSettingsDialog::~NetworkSettingsDialog()
{
    delete ui;
}

void NetworkSettingsDialog::accept()
{
    writeSettings();
    QDialog::accept();
}

void NetworkSettingsDialog::showEvent(QShowEvent *event)
{
    readSettings();
    event->accept();
}

void NetworkSettingsDialog::readSettings()
{

    QSettings settings;
    settings.beginGroup("Network");
    QString ifaceName = settings.value("interface", QString()).toString();
    QString address = settings.value("address", QSTR_ADDRESS).toString();
    settings.endGroup();

    ui->txtAddress->setText(address);
    ui->comboInterface->addItem("Any", "");
    foreach( const QNetworkInterface& iface,  QNetworkInterface::allInterfaces() ) {
        if ( iface.isValid() && ((iface.flags() & QNetworkInterface::IsLoopBack) == 0) ) {
            QString name = iface.name();
            QString text = iface.humanReadableName();
            ui->comboInterface->addItem(text, name);
            if (name == ifaceName) {
                ui->comboInterface->setCurrentText(text);
            }
        }
    }

}

void NetworkSettingsDialog::writeSettings()
{
    QSettings settings;
    QString networkAddr = QSTR_ADDRESS;
    QString networkIface;

    networkAddr = ui->txtAddress->text();
    networkIface = ui->comboInterface->currentData().toString();

    settings.beginGroup("Network");
    settings.setValue("interface", networkIface);
    settings.setValue("address", networkAddr);
    settings.endGroup();
    settings.sync();
}

void NetworkSettingsDialog::restoreDefaults()
{
    ui->txtAddress->setText(QSTR_ADDRESS);
    ui->comboInterface->setCurrentText(tr("Any"));
}
