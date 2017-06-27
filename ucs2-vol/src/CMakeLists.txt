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
PROJECT_TARGET_ADD(ucs2-vol)

# Define targets source files
ADD_LIBRARY(ucs2-vol STATIC callbacks.cpp device_container.cpp  device_value.cpp  libmostvolume.cpp  setup.cpp)

    # Expose Library Properties
    SET_TARGET_PROPERTIES(ucs2-vol PROPERTIES OUTPUT_NAME ucs2vol)

    # Library dependencies from PKG_REQUIRED_LIST
    TARGET_LINK_LIBRARIES(ucs2-vol     # Library dependencies (include updates automatically)
        ucs2-lib
        ${link_libraries}
    )

    # Define properties to expose when others use this target
    TARGET_INCLUDE_DIRECTORIES(ucs2-vol
       PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../cfg
        ${CMAKE_CURRENT_SOURCE_DIR}/ucs-xml
    )

