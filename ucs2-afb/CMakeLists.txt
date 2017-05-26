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
PROJECT_TARGET_ADD(ucs2)

    # Define project Targets
    ADD_LIBRARY(ucs2 MODULE ucs_binding.c ucs_apihat.c)

    # Binder exposes a unique public entry point
    SET_TARGET_PROPERTIES(ucs2 PROPERTIES
	PREFIX "afb-"
        LABELS "BINDING"
	LINK_FLAGS  ${BINDINGS_LINK_FLAG}
        OUTPUT_NAME ${TARGET_NAME}

    )

    # Library dependencies (include updates automatically)
    TARGET_LINK_LIBRARIES(ucs2 
        ucs2-inter
        ${link_libraries}
    )

    # installation directory
    INSTALL(TARGETS ucs2
        LIBRARY DESTINATION ${BINDINGS_INSTALL_DIR})

