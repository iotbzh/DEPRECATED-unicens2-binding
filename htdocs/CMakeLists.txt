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
# HTML Testing Files
##################################################
PROJECT_TARGET_ADD(www_test)

    file(GLOB SOURCE_FILES "*.html" "*.js" "*.jpg")

    add_custom_target(${TARGET_NAME}
       DEPENDS  ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}
    )

    add_custom_command(
       DEPENDS  ${SOURCE_FILES}
       OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}
       COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}
       COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}
       COMMAND cp -r ${SOURCE_FILES} ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}
    )

    SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
	LABELS "HTDOCS"
	OUTPUT_NAME ${TARGET_NAME}
    )
