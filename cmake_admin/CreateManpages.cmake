#[=======================================================================[
Copyright © 2019-2024 Pedro López-Cabanillas <plcl@users.sf.net>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

* Neither the name of Kitware, Inc. nor the names of its
  contributors may be used to endorse or promote products derived
  from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#]=======================================================================]

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
              DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 )
ENDMACRO(CREATE_MANPAGES)
