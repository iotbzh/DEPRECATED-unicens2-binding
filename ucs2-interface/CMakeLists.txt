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

# Add target to project dependency list
PROJECT_TARGET_ADD(ucs2-inter)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNDEBUG")

	# Search for libs
	find_package (LibXml2 REQUIRED)
    
	# Define targets
    ADD_LIBRARY(ucs2-inter STATIC ucs_lib_interf.c ucs-xml/UcsXml.c ucs-xml/UcsXml_Private.c)

    # Library properties
    SET_TARGET_PROPERTIES(ucs2-inter PROPERTIES OUTPUT_NAME ucs2interface)

    # Depends on Unicens2 lib
    TARGET_LINK_LIBRARIES(ucs2-inter ucs2-lib ${LIBXML2_LIBRARIES})

    # Define includes
    TARGET_INCLUDE_DIRECTORIES(ucs2-inter
        PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR} ${LIBXML2_INCLUDE_DIR}
    )
