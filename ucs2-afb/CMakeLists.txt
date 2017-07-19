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


# Generate API-v2 hat from OpenAPI json definition
macro(SET_TARGET_GENSKEL TARGET_NAME API_DEF_NAME)
    add_custom_command(OUTPUT ${API_DEF_NAME}.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${API_DEF_NAME}.json
        COMMAND afb-genskel  ${API_DEF_NAME}.json >${API_DEF_NAME}.h 
    )
    add_custom_target(${API_DEF_NAME}_OPENAPI DEPENDS ${API_DEF_NAME}.h)
    add_dependencies(${TARGET_NAME} ${API_DEF_NAME}_OPENAPI)
    
endmacro(SET_TARGET_GENSKEL)

# Add target to project dependency list
PROJECT_TARGET_ADD(ucs2-afb)

    # Define project Targets
    ADD_LIBRARY(${TARGET_NAME} MODULE ucs_apihat.c ucs_binding.c )

    # Generate API-v2 hat from OpenAPI json definition
    SET_TARGET_GENSKEL(${TARGET_NAME} ucs_apidef)

    # Binder exposes a unique public entry point
    SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
	PREFIX "afb-"
        LABELS "BINDING"
	LINK_FLAGS  ${BINDINGS_LINK_FLAG}
        OUTPUT_NAME ucs2

    )

    # Library dependencies (include updates automatically)
    TARGET_LINK_LIBRARIES(${TARGET_NAME}
        ucs2-inter
        ${link_libraries}
    )

    # installation directory
    INSTALL(TARGETS ${TARGET_NAME}
        LIBRARY DESTINATION ${BINDINGS_INSTALL_DIR})

