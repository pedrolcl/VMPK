#[===========================================================================[
Virtual MIDI Piano Keyboard
Copyright (C) 2008-2024 Pedro Lopez-Cabanillas <plcl@users.sourceforge.net>

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
#]===========================================================================]

if (BUILD_DOCS)
    find_program(XSLTPROC_EXECUTABLE xsltproc)
endif()

if (XSLTPROC_EXECUTABLE)
    message(STATUS "XSLTPROC Found: ${XSLTPROC_EXECUTABLE}")
    include(CreateManpages)
    create_manpages(vmpk.xml)
else()
    install( FILES vmpk.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 )
endif()
