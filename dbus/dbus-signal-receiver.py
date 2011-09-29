#!/usr/bin/env python

import sys
import gobject
import dbus
import dbus.mainloop.glib

def noteon_handler(note):
    print "note on %d" % note

def noteoff_handler(note):
    print "note off %d" % note

if __name__ == '__main__':
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
    bus = dbus.SessionBus()
    
    try:
        object  = bus.get_object("net.sourceforge.vmpk", "/")
        object.connect_to_signal("event_noteon", noteon_handler, dbus_interface="net.sourceforge.vmpk")
        object.connect_to_signal("event_noteoff", noteoff_handler, dbus_interface="net.sourceforge.vmpk")
    except dbus.DBusException:
        print "please, run VMPK before this program..."
        sys.exit(1)
    
    loop = gobject.MainLoop()
    loop.run()
