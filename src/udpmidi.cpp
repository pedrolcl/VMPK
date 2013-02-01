/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2013, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

/*
 *  This code was based on Qmidinet, by Rui Nuno Capela: http://qmidinet.sf.net
 *  later overhauled with Qt 4.8 support for UDP multicast (QUdpSocket).
 */

#if defined(NETWORK_MIDI)

#include <sstream>

#include <QByteArray>
#include <QThread>
#include <QNetworkInterface>
#include <QUdpSocket>
#include <QDebug>

#include "udpmidi.h"
#include "netsettings.h"
#include "constants.h"

const QHostAddress MULTICAST_ADDRESS(QSTR_MULTICAST_ADDRESS);

struct NetworkMidiData {
    NetworkMidiData(): socket(0) {}
    QUdpSocket *socket;
};

/* NetMidiIn */

NetMidiIn :: NetMidiIn( const std::string clientName, unsigned int queueSizeLimit ) :
    QObject(0), RtMidiIn(queueSizeLimit)
{
    initialize( clientName );
}

void NetMidiIn ::initialize( const std::string& /*clientName*/ )
{
    NetworkMidiData *data = new NetworkMidiData;
    apiData_ = (void *) data;
    inputData_.apiData = (void *) data;
}

NetMidiIn :: ~NetMidiIn()
{
    // Close a connection if it exists.
    closePort();
    // Cleanup.
    NetworkMidiData *data = (NetworkMidiData *) inputData_.apiData ;
    delete data;
}

void NetMidiIn ::openPort( unsigned int /*portNumber*/, const std::string /*portName*/ )
{
    NetworkMidiData *data = static_cast<NetworkMidiData *> (apiData_);
    QNetworkInterface iface = NetworkSettings::instance().iface();
    int udpPort = NetworkSettings::instance().port();

    data->socket = new QUdpSocket();
    data->socket->bind(udpPort, QUdpSocket::ShareAddress);
    data->socket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);
    data->socket->setSocketOption(QAbstractSocket::MulticastTtlOption, 1);

    if (iface.isValid()) {
        data->socket->setMulticastInterface(iface);
    }
    data->socket->joinMulticastGroup(MULTICAST_ADDRESS);
    connect(data->socket, SIGNAL(readyRead()), this, SLOT(processIncomingMessages()));
    inputData_.doInput = true;
}

void NetMidiIn ::openVirtualPort( const std::string /*portName*/ )
{
    errorString_ = "NetMidiIn::openVirtualPort: cannot be implemented in UDP!";
    error( RtError::WARNING );
}

unsigned int NetMidiIn ::getPortCount()
{
    return 1;
}

std::string NetMidiIn ::getPortName( unsigned int /*portNumber*/ )
{
    std::ostringstream ost;
    ost << "UDP/" << NetworkSettings::instance().port();
    return ost.str();
}

void NetMidiIn ::closePort()
{
    NetworkMidiData *data = static_cast<NetworkMidiData *> (apiData_);
    inputData_.doInput = false;
    // close and delete socket
    delete data->socket;
    data->socket = 0;
}

void NetMidiIn ::processIncomingMessages()
{
    NetworkMidiData *data = static_cast<NetworkMidiData *> (apiData_);
    QUdpSocket *socket = data->socket;
    RtMidiIn::MidiMessage message;
    if (inputData_.doInput) {
        while (socket->hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(socket->pendingDatagramSize());
            socket->readDatagram(datagram.data(), datagram.size());
            message.timeStamp = 0;
            message.bytes.clear();
            message.bytes.assign(datagram.begin(), datagram.end());
            if ( inputData_.usingCallback ) {
                RtMidiIn::RtMidiCallback callback =
                        (RtMidiIn::RtMidiCallback) inputData_.userCallback;
                callback(message.timeStamp, &message.bytes, inputData_.userData);
            } else {
                if ( inputData_.queue.size < inputData_.queue.ringSize ) {
                  inputData_.queue.ring[inputData_.queue.back++] = message;
                  if ( inputData_.queue.back == inputData_.queue.ringSize )
                    inputData_.queue.back = 0;
                  inputData_.queue.size++;
                } else {
                  qWarning() << "NetMidiIn: message queue limit reached!!\n\n";
                }
            }
        }
    }
}

/* NetMidiOut */

NetMidiOut :: NetMidiOut( const std::string clientName ) : RtMidiOut()
{
    initialize(clientName);
}

void NetMidiOut ::initialize( const std::string& /*clientName*/ )
{
    NetworkMidiData *data = new NetworkMidiData;
    apiData_ = (void *) data;
}

NetMidiOut :: ~NetMidiOut()
{
    // Close a connection if it exists.
    closePort();
    // Cleanup.
    NetworkMidiData *data = (NetworkMidiData *) apiData_;
    delete data;
}

void NetMidiOut ::openPort( unsigned int /*portNumber*/, const std::string /*portName*/ )
{
    NetworkMidiData *data = static_cast<NetworkMidiData *> (apiData_);
    data->socket = new QUdpSocket();
}

void NetMidiOut ::openVirtualPort( const std::string /*portName*/ )
{
    errorString_ = "NetMidiOut::openVirtualPort: cannot be implemented in UDP!";
    error( RtError::WARNING );
}

unsigned int NetMidiOut ::getPortCount()
{
    return 1;
}

std::string NetMidiOut ::getPortName( unsigned int /*portNumber*/ )
{
    std::ostringstream ost;
    ost << "UDP/" << NetworkSettings::instance().port();
    return ost.str();
}

void NetMidiOut ::closePort()
{
    NetworkMidiData *data = static_cast<NetworkMidiData *> (apiData_);
    delete data->socket;
    data->socket = 0;
}

void NetMidiOut ::sendMessage( std::vector<unsigned char> *message )
{
    NetworkMidiData *data = static_cast<NetworkMidiData *> (apiData_);
    if (data->socket <= 0) {
        qDebug() << "socket = " << data->socket;
        return;
    }
    data->socket->writeDatagram((char *) &((*message)[0]), message->size(), MULTICAST_ADDRESS, NetworkSettings::instance().port());
}

#endif
