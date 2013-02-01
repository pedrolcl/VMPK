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

#ifndef UDPMIDI_H
#define UDPMIDI_H

#if defined(NETWORK_MIDI)

#include <QObject>
#include <RtMidi.h>

class NetMidiIn : public QObject, public RtMidiIn
{

  Q_OBJECT

public:

  //! Default constructor that allows an optional client name.
  /*!
      An exception will be thrown if a MIDI system initialization error occurs.
  */
  NetMidiIn( const std::string clientName = std::string( "Network Input Client"), unsigned int queueSizeLimit = 100 );

  //! If a MIDI connection is still open, it will be closed by the destructor.
  ~NetMidiIn();

  //! Open a MIDI input connection.
  /*!
      An optional port number greater than 0 can be specified.
      Otherwise, the default or first port found is opened.
  */
  void openPort( unsigned int portNumber = 0, const std::string Portname = std::string( "Network Input" ) );

  //! Create a virtual input port, with optional name, to allow software connections (OS X and ALSA only).
  /*!
      This function creates a virtual MIDI input port to which other
      software applications can connect.  This type of functionality
      is currently only supported by the Macintosh OS-X and Linux ALSA
      APIs (the function does nothing for the other APIs).
  */
  void openVirtualPort( const std::string portName = std::string( "Network Input" ) );

  //! Close an open MIDI connection (if one exists).
  void closePort( void );

  //! Return the number of available MIDI input ports.
  unsigned int getPortCount();

  //! Return a string identifier for the specified MIDI input port number.
  /*!
      An empty string is returned if an invalid port specifier is provided.
  */
  std::string getPortName( unsigned int portNumber = 0 );

public slots:

  void processIncomingMessages();

private:

  void initialize( const std::string& clientName );

};

class NetMidiOut : public RtMidiOut
{
 public:

  //! Default constructor that allows an optional client name.
  /*!
      An exception will be thrown if a MIDI system initialization error occurs.
  */
  NetMidiOut( const std::string clientName = std::string( "Network Midi Output Client" ) );

  //! The destructor closes any open MIDI connections.
  ~NetMidiOut();

  //! Open a MIDI output connection.
  /*!
      An optional port number greater than 0 can be specified.
      Otherwise, the default or first port found is opened.  An
      exception is thrown if an error occurs while attempting to make
      the port connection.
  */
  void openPort( unsigned int portNumber = 0, const std::string portName = std::string( "Network Output" ) );

  //! Close an open MIDI connection (if one exists).
  void closePort();

  //! Create a virtual output port, with optional name, to allow software connections (OS X and ALSA only).
  /*!
      This function creates a virtual MIDI output port to which other
      software applications can connect.  This type of functionality
      is currently only supported by the Macintosh OS-X and Linux ALSA
      APIs (the function does nothing with the other APIs).  An
      exception is thrown if an error occurs while attempting to create
      the virtual port.
  */
  void openVirtualPort( const std::string portName = std::string( "Network Output" ) );

  //! Return the number of available MIDI output ports.
  unsigned int getPortCount();

  //! Return a string identifier for the specified MIDI port type and number.
  /*!
      An empty string is returned if an invalid port specifier is provided.
  */
  std::string getPortName( unsigned int portNumber = 0 );

  virtual void sendMessage( std::vector<unsigned char> *message );

  void initialize( const std::string& clientName );
};
#endif // defined(NETWORK_MIDI)


#endif // UDPMIDI_H
