/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2012, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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
 *  later overhauled with Qt 4.8 support for UDP multicast.
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

const QHostAddress MULTICAST_ADDRESS("225.0.0.37");

struct NetworkMidiData {
    NetworkMidiData(): socket(0), thread(0) { }
    QUdpSocket *socket;
    class UdpDeviceThread *thread;
};

/* NetMidiIn */

class UdpDeviceThread : public QThread
{
public:
        UdpDeviceThread(RtMidiIn::RtMidiInData *data)
            : QThread(), m_data(data)
        { }

protected:
        void run();

private:
        RtMidiIn::RtMidiInData *m_data;
};

void UdpDeviceThread::run (void)
{
    NetworkMidiData *apiData = static_cast<NetworkMidiData *> (m_data->apiData);
    RtMidiIn::MidiMessage message;
    QUdpSocket *socket = apiData->socket;
    while (m_data->doInput) {
        while (socket->hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(socket->pendingDatagramSize());
            socket->readDatagram(datagram.data(), datagram.size());
            message.timeStamp = 0;
            message.bytes.clear();
            message.bytes.assign(datagram.begin(), datagram.end());
            if ( m_data->usingCallback ) {
                RtMidiIn::RtMidiCallback callback =
                        (RtMidiIn::RtMidiCallback) m_data->userCallback;
                callback(message.timeStamp, &message.bytes, m_data->userData);
            } else {
                if ( m_data->queue.size < m_data->queue.ringSize ) {
                  m_data->queue.ring[m_data->queue.back++] = message;
                  if ( m_data->queue.back == m_data->queue.ringSize )
                    m_data->queue.back = 0;
                  m_data->queue.size++;
                } else {
                  qDebug() << "RtMidiIn: message queue limit reached!!\n\n";
                }
            }
        }
        socket->waitForReadyRead(1000);
    }
}

NetMidiIn :: NetMidiIn( const std::string clientName, unsigned int queueSizeLimit ) : RtMidiIn(queueSizeLimit)
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

    if (iface.isValid())
        data->socket->setMulticastInterface(iface);

    data->socket->joinMulticastGroup(MULTICAST_ADDRESS);

    // start the input thread
    data->thread = new UdpDeviceThread(&inputData_);
    inputData_.doInput = true;
    data->thread->start();
    if (data->thread == 0 || !data->thread->isRunning()) {
        inputData_.doInput = false;
        errorString_ = "RtMidiIn::openPort: error starting MIDI input thread!";
        error( RtError::THREAD_ERROR );
    }
}

void NetMidiIn ::openVirtualPort( const std::string /*portName*/ )
{
    errorString_ = "RtMidiIn::openVirtualPort: cannot be implemented in UDP!";
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
    //qDebug() << Q_FUNC_INFO;
    NetworkMidiData *data = static_cast<NetworkMidiData *> (apiData_);
    // Shutdown the input thread
    if (data->thread != 0) {
        if (data->thread->isRunning()) {
            inputData_.doInput = false;
            data->thread->wait(1200); // Timeout>1sec.
        }
        delete data->thread;
        data->thread = 0;
    }
    // close and delete socket
    delete data->socket;
    data->socket = 0;
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
    // Turn off loopback...
    data->socket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);
}

void NetMidiOut ::openVirtualPort( const std::string /*portName*/ )
{
    errorString_ = "RtMidiOut::openVirtualPort: cannot be implemented in UDP!";
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
    //qDebug() << Q_FUNC_INFO;
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
