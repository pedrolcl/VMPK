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

IF(WINDRES_EXECUTABLE)
    SET(WINDRES_FOUND TRUE)
ELSE(WINDRES_EXECUTABLE)
    FIND_PROGRAM(WINDRES_EXECUTABLE
    NAMES windres mingw32-windres i686-mingw32-windres) 
    IF(WINDRES_EXECUTABLE)
        SET(WINDRES_FOUND TRUE)
    ELSE(WINDRES_EXECUTABLE)
    IF(NOT WINDRES_FIND_QUIETLY)
        IF(WINDRES_FIND_REQUIRED)
            MESSAGE(FATAL_ERROR "windres program couldn't be found")
        ENDIF(WINDRES_FIND_REQUIRED)
    ENDIF(NOT WINDRES_FIND_QUIETLY)
    ENDIF(WINDRES_EXECUTABLE)
ENDIF (WINDRES_EXECUTABLE)

# ADD_WINDRES_OBJS(outfiles inputfile ... )

MACRO(ADD_WINDRES_OBJS outfiles)
    FOREACH (it ${ARGN})
        GET_FILENAME_COMPONENT(outfile ${it} NAME_WE)
        GET_FILENAME_COMPONENT(infile ${it} ABSOLUTE)
        SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/${outfile}.obj)
        ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
            COMMAND ${WINDRES_EXECUTABLE} 
            ARGS -I${CMAKE_CURRENT_SOURCE_DIR}
                 -i ${infile}
                 -o ${outfile}
            MAIN_DEPENDENCY ${infile})
        SET(${outfiles} ${${outfiles}} ${outfile})
    ENDFOREACH (it)
ENDMACRO(ADD_WINDRES_OBJS)
