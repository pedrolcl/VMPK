#!/bin/bash
# London Tower Big Ben tune as a simple bash script
# Copyright (C) 2002-2013 Pedro Lopez-Cabanillas <plcl@users.sf.net>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

tune="60 4, 64 4, 62 4, 55 2, 60 4, 62 4, 64 4, 60 2, \
      64 4, 60 4, 62 4, 55 2, 55 4, 62 4, 64 4, 60 2,"

DBUSC=$(which qdbus)
tempo=80

function dbus_command() {
    $DBUSC net.sourceforge.vmpk / $* >/dev/null 2>&1
}

function playnote() {
	let "ms = 240000 / ($2 * $tempo)"
	dbus_command noteon $1
	sleep $ms'e-3s'
	dbus_command noteoff $1
}

function playtune() {
	echo $tune | while read -rd, note length; do
		playnote $note $length
	done
}

function playtime() {
	h=$(date +%l)
	while [ $h -gt 0 ]; do
	    playnote 43 2
	    let "h--"
	done
}

if [ -z $DBUSC ]; then
    echo "qdbus (from Qt4) is required"
else
    if [ -z $($DBUSC | grep "net.sourceforge.vmpk") ]; then
        echo "please run VMPK before this script"
    else
        # volume = 100
    	dbus_command controlchange 7 100
    	# instrument = bells
    	dbus_command programchange 14
    	playtune
    	sleep 2
    	playtime
	fi
fi
