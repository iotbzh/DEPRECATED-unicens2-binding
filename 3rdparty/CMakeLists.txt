###########################################################################
# Copyright 2015, 2016, 2017 IoT.bzh
#
# author: Fulup Ar Foll <fulup@iot.bzh>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###########################################################################

set(MXML external-mxml)
set(MXML_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mxml)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
ExternalProject_Add(${MXML}
    GIT_REPOSITORY https://github.com/michaelrsweet/mxml.git
    GIT_TAG release-2.10
    SOURCE_DIR ${MXML_SOURCE_DIR}
    CONFIGURE_COMMAND ./configure --build x86_64 --host aarch64
    BUILD_COMMAND make libmxml.so.1.5
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
)

PROJECT_TARGET_ADD(mxml)

add_library(${TARGET_NAME} SHARED IMPORTED GLOBAL)

SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
    LABELS LIBRARY
    IMPORTED_LOCATION ${MXML_SOURCE_DIR}/libmxml.so.1
    INTERFACE_INCLUDE_DIRECTORIES ${MXML_SOURCE_DIR}
)

add_dependencies(${TARGET_NAME} ${MXML})
