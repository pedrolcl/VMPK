#!/usr/bin/env python

import sys
import time
import dbus

if __name__ == '__main__':
    bus = dbus.SessionBus()
    try:
        object = bus.get_object("net.sourceforge.vmpk", "/")
    except dbus.DBusException:
        print "please, run VMPK before this program..."
        sys.exit(1)

    # interface methods
    interface = dbus.Interface(object, "net.sourceforge.vmpk")
    #interface.programchange(73);
    interface.programnamechange("flute");
    interface.noteon(69);
    time.sleep(1);
    interface.noteoff(69);

    # introspection
    print object.Introspect(dbus_interface="org.freedesktop.DBus.Introspectable")

    # close vmpk
    interface.quit()
