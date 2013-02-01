# Virtual MIDI Piano Keyboard
# Copyright (C) 2008-2013 Pedro Lopez-Cabanillas <plcl@users.sourceforge.net>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along 
# with this program; If not, see <http://www.gnu.org/licenses/>.

FIND_PROGRAM(XSLTPROC_EXECUTABLE xsltproc)
IF(XSLTPROC_EXECUTABLE)
    MESSAGE(STATUS "XSLTPROC Found: ${XSLTPROC_EXECUTABLE}")
    INCLUDE(CreateManpages)
    CREATE_MANPAGES(vmpk.xml)
ELSE(XSLTPROC_EXECUTABLE)
    MESSAGE(STATUS "XSLTPROC NOT Found")
    INSTALL( FILES vmpk.1 DESTINATION share/man/man1 )
ENDIF(XSLTPROC_EXECUTABLE)
