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



##################################################
# UCS2 Config file
##################################################
PROJECT_TARGET_ADD(ucs2_config)

    file(GLOB XML_FILES "*.xml")
    set(XML_SCHEMA unicens.xsd)

    add_custom_target(${TARGET_NAME}
       DEPENDS  ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}
    )

    # check XML schema before pushing config
    add_custom_command(
       DEPENDS  ${XML_FILES}
       OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}
       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
       COMMAND xmllint -schema ${XML_SCHEMA} ${XML_FILES} --noout
       COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}
       COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}      
       COMMAND cp -r ${XML_FILES} ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}
    )

    SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
	LABELS "DATA"
	OUTPUT_NAME ${TARGET_NAME}
    )
