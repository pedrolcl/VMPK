/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2011, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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
 *  This code is based on Qmidinet, by Rui Nuno Capela: http://qmidinet.sf.net
 */

#if defined(NETWORK_MIDI)

#include <cstring>
#include <sstream>

#if defined(__SYMBIAN32__)
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <unistd.h>
#else
#if defined(WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>
#endif
#endif

#include <QThread>
#include <QDebug>
#include "RtMidi.h"
#include "preferences.h"

int g_iUdpPort = NETWORKPORTNUMBER;

struct NetworkMidiData {
    NetworkMidiData(): socket(0), thread(0) { }
    int socket;
    struct sockaddr_in sockaddr;
    class UdpDeviceThread *thread;
};

#if defined(WIN32)
static WSADATA g_wsaData;
typedef int socklen_t;

static void __attribute__((constructor)) startup()
{
    qDebug() << "startup";
    WSAStartup(MAKEWORD(1, 1), &g_wsaData);
}

static void __attribute__((destructor)) cleanup()
{
    qDebug() << "cleanup";
    WSACleanup();
}
#endif

/* RtMidiIn */

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
    qDebug() << "running!";
    while (m_data->doInput) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(apiData->socket, &fds);
        int fdmax = apiData->socket;
        struct timeval tv;
        tv.tv_sec  = 1; // timeout period (1 second)...
        tv.tv_usec = 0;
        int s = ::select(fdmax + 1, &fds, NULL, NULL, &tv);
        if (s < 0) {
            qDebug() << "RtMidiIn: select = " << s << ": " << ::strerror(s);
            break;
        }
        if (s == 0) {
            //qDebug() << "\nRtMidiIn: timeout";
            continue;
        }
        // A Network event
        if (FD_ISSET(apiData->socket, &fds)) {
            // Read from network...
            unsigned char buf[1024];
            struct sockaddr_in sender;
            socklen_t slen = sizeof(sender);
            int r = ::recvfrom(apiData->socket, (char *) buf, sizeof(buf),
                        0, (struct sockaddr *) &sender, &slen);
            if (r > 0) {
                message.timeStamp = 0;
                message.bytes.clear();
                for ( int i = 0; i < r; ++i ) {
                      message.bytes.push_back( buf[i] );
                }
                if ( m_data->usingCallback ) {
                    RtMidiIn::RtMidiCallback callback =
                            (RtMidiIn::RtMidiCallback) m_data->userCallback;
                    callback(message.timeStamp, &message.bytes, m_data->userData);
                } else {
                    if ( m_data->queueLimit > m_data->queue.size() ) {
                      m_data->queue.push( message );
                    } else {
                      qDebug() << "RtMidiIn: message queue limit reached!!\n\n";
                    }
                }
            } else if (r < 0) {
                qDebug() << "RtMidiIn: recvfrom=" << r;
            }
        }
    }
}

void RtMidiIn :: initialize( const std::string& /*clientName*/ )
{
    NetworkMidiData *data = new NetworkMidiData;
    apiData_ = (void *) data;
    inputData_.apiData = (void *) data;
}

RtMidiIn :: ~RtMidiIn()
{
    // Close a connection if it exists.
    closePort();
    // Cleanup.
    NetworkMidiData *data = (NetworkMidiData *) inputData_.apiData ;
    delete data;
}

void RtMidiIn :: openPort( unsigned int /*portNumber*/, const std::string /*portName*/ )
{
    NetworkMidiData *data = static_cast<NetworkMidiData *> (apiData_);
    // Setup network protocol...
    data->socket = ::socket(PF_INET, SOCK_DGRAM, 0);
    if (data->socket < 0) {
        qDebug() << "socket(in)";
        return;
    }
    struct sockaddr_in addrin;
    ::memset(&addrin, 0, sizeof(addrin));
    addrin.sin_family = AF_INET;
    addrin.sin_addr.s_addr = htonl(INADDR_ANY);
    addrin.sin_port = htons(g_iUdpPort);
    if (::bind(data->socket, (struct sockaddr *) (&addrin), sizeof(addrin)) < 0) {
        qDebug() << "bind";
        return;
    }
    // Will Hall, 2007
    // INADDR_ANY will bind to default interface,
    struct in_addr if_addr_in;
    if_addr_in.s_addr = htonl(INADDR_ANY);
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = ::inet_addr("225.0.0.37");
    mreq.imr_interface.s_addr = if_addr_in.s_addr;
    if (::setsockopt (data->socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                     (char *) &mreq, sizeof(mreq)) < 0) {
        qDebug() << "setsockopt(IP_ADD_MEMBERSHIP)";
        errorString_ = "RtMidiIn::openPort: OS is probably missing multicast support.";
        error( RtError::SYSTEM_ERROR );
    }
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

void RtMidiIn :: openVirtualPort( const std::string /*portName*/ )
{
    errorString_ = "RtMidiIn::openVirtualPort: cannot be implemented in UDP!";
    error( RtError::WARNING );
}

unsigned int RtMidiIn :: getPortCount()
{
    return 1;
}

std::string RtMidiIn :: getPortName( unsigned int /*portNumber*/ )
{
    std::ostringstream ost;
    ost << "UDP/" << g_iUdpPort;
    return ost.str();
}

void RtMidiIn :: closePort()
{
    NetworkMidiData *data = static_cast<NetworkMidiData *> (apiData_);
    // Shutdown the input thread.
    if (data->thread != 0) {
        if (data->thread->isRunning()) {
            inputData_.doInput = false;
            data->thread->wait(1200); // Timeout>1sec.
        }
        delete data->thread;
        data->thread = 0;
    }
    // close socket
    if (data->socket >= 0) {
#if defined(WIN32)
        ::closesocket(data->socket);
#else
        ::close(data->socket);
#endif
        data->socket = 0;
    }
}

/* RtMidiOut */

void RtMidiOut :: initialize( const std::string& /*clientName*/ )
{
    NetworkMidiData *data = new NetworkMidiData;
    apiData_ = (void *) data;
}

RtMidiOut :: ~RtMidiOut()
{
    // Close a connection if it exists.
    closePort();
    // Cleanup.
    NetworkMidiData *data = (NetworkMidiData *) apiData_;
    delete data;
}

void RtMidiOut :: openPort( unsigned int /*portNumber*/, const std::string /*portName*/ )
{
    NetworkMidiData *data = static_cast<NetworkMidiData *> (apiData_);
    data->socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (data->socket < 0) {
        errorString_ = "RtMidiOut::openPort: error creating a socket";
        error( RtError::SYSTEM_ERROR );
    }
    ::memset(&data->sockaddr, 0, sizeof(data->sockaddr));
    data->sockaddr.sin_family = AF_INET;
    data->sockaddr.sin_addr.s_addr = ::inet_addr("225.0.0.37");
    data->sockaddr.sin_port = htons(g_iUdpPort);
    // Turn off loopback...
    int loop = 0;
    if (::setsockopt(data->socket, IPPROTO_IP, IP_MULTICAST_LOOP,
                    (char *) &loop, sizeof (loop)) < 0) {
        qDebug() << "setsockopt(IP_MULTICAST_LOOP)";
        return;
    }
}

void RtMidiOut :: openVirtualPort( const std::string /*portName*/ )
{
    errorString_ = "RtMidiOut::openVirtualPort: cannot be implemented in UDP!";
    error( RtError::WARNING );
}

unsigned int RtMidiOut :: getPortCount()
{
    return 1;
}

std::string RtMidiOut :: getPortName( unsigned int /*portNumber*/ )
{
    std::ostringstream ost;
    ost << "UDP/" << g_iUdpPort;
    return ost.str();
}

void RtMidiOut :: closePort()
{
    NetworkMidiData *data = static_cast<NetworkMidiData *> (apiData_);
    if (data->socket >= 0) {
#if defined(WIN32)
        ::closesocket(data->socket);
#else
        ::close(data->socket);
#endif
        data->socket = 0;
    }
}

void RtMidiOut :: sendMessage( std::vector<unsigned char> *message )
{
    NetworkMidiData *data = static_cast<NetworkMidiData *> (apiData_);
    if (data->socket <= 0) {
        qDebug() << "socket = " << data->socket;
        return;
    }
    if (::sendto(data->socket, (char *) &((*message)[0]), message->size(), 0,
            (struct sockaddr*) &data->sockaddr, sizeof(data->sockaddr)) < 0) {
        qDebug() << "sendto";
        return;
    }
}

#endif
