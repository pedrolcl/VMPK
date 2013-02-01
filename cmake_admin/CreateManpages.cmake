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

MACRO(CREATE_MANPAGES)
    SET(outfiles)
    FOREACH (it ${ARGN})
        GET_FILENAME_COMPONENT(outfile ${it} NAME_WE)
        GET_FILENAME_COMPONENT(infile ${it} ABSOLUTE)
        SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/${outfile}.1)
        SET(outfiles ${outfiles} ${outfile})
        ADD_CUSTOM_COMMAND( OUTPUT ${outfile}
            COMMAND ${XSLTPROC_EXECUTABLE}
                --nonet --xinclude --xincludestyle  
                --output ${outfile} 
                http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl
                ${infile} 
            DEPENDS ${infile})
    ENDFOREACH (it)
    ADD_CUSTOM_TARGET(manpages ALL DEPENDS ${outfiles})
    INSTALL ( FILES ${outfiles}
              DESTINATION share/man/man1 )
ENDMACRO(CREATE_MANPAGES)
