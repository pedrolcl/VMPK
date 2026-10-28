#=======================================================================
# Copyright © 2021-2026 Pedro López-Cabanillas <plcl@users.sf.net>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the name of Kitware, Inc. nor the names of its
#   contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=======================================================================

if(NOT EXISTS ${PANDOC_EXECUTABLE})
    find_program(PANDOC_EXECUTABLE pandoc)
    mark_as_advanced(PANDOC_EXECUTABLE)
    if(NOT EXISTS ${PANDOC_EXECUTABLE})
        message(WARNING "Pandoc not found. Install Pandoc or set the cache variable PANDOC_EXECUTABLE.")
    endif()
endif()

if (EXISTS ${PANDOC_EXECUTABLE})
    macro(add_manpage out_file)
        set(_src  ${CMAKE_CURRENT_SOURCE_DIR}/${out_file}.md)
        if (NOT PROJECT_RELEASE_DATE)
            unset(_date)
            execute_process (
                COMMAND bash -c "LANG=en;date +'%B %d, %Y'"
                OUTPUT_VARIABLE _date
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        else()
            set(_date ${PROJECT_RELEASE_DATE})
        endif()
        set(_footer "${PROJECT_NAME} ${PROJECT_VERSION}")
        add_custom_command (
            OUTPUT ${out_file}
            COMMAND ${PANDOC_EXECUTABLE} -s -t man -Vdate=${_date} -Vfooter=${_footer} ${_src} -o ${out_file}
            DEPENDS ${_src}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            VERBATIM
        )
        add_custom_target(manpage ALL DEPENDS ${out_file})
    endmacro()
endif()
