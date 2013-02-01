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

#ifndef NETSETTINGS_H
#define NETSETTINGS_H

#include <QNetworkInterface>

class NetworkSettings
{
public:
    static NetworkSettings& instance()
    {
        static NetworkSettings instance;
        return instance;
    }

    int port() { return m_port; }
    void setPort(const int port) { m_port = port; }

    QNetworkInterface& iface() { return m_iface; }
    void setIface(const QNetworkInterface& iface) { m_iface = iface; }

private:
    NetworkSettings() {}
    //NetworkSettings(const NetworkSettings& s);
    NetworkSettings(NetworkSettings& s);
    const NetworkSettings& operator=(NetworkSettings &s);
    virtual ~NetworkSettings() {}

    int m_port;
    QNetworkInterface m_iface;
};

#endif // NETSETTINGS_H
