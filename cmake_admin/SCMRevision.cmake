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

find_package(Subversion QUIET)
if (Subversion_FOUND)
    Subversion_WC_INFO(${PROJECT_SOURCE_DIR} PROJECT IGNORE_SVN_FAILURE)
    if (DEFINED PROJECT_WC_REVISION)
        message(STATUS "Current revision (SVN) is ${PROJECT_WC_REVISION}")
    endif()
endif()

if (NOT DEFINED PROJECT_WC_REVISION)
    find_package(Git QUIET)
    if (Git_FOUND)
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            RESULT_VARIABLE res
            OUTPUT_VARIABLE PROJECT_WC_REVISION
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        if (${res} EQUAL 0)
            message(STATUS "Current revision (Git) is ${PROJECT_WC_REVISION}")
        else()
            unset(PROJECT_WC_REVISION)
        endif()
    endif()
endif()

if (DEFINED PROJECT_WC_REVISION)
    set(${PROJECT_NAME}_WC_REVISION ${PROJECT_WC_REVISION})
endif()
