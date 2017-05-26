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
PROJECT_TARGET_ADD(ucs2-lib)

# Define targets source files
ADD_LIBRARY(ucs2-lib STATIC ucs_alm.c ucs_amd.c ucs_ams.c ucs_amsmessage.c ucs_amspool.c ucs_amtp.c ucs_attach.c ucs_base.c ucs_bc_diag.c ucs_class.c ucs_cmd.c ucs_dec.c ucs_dl.c ucs_eh.c ucs_encoder.c ucs_epm.c ucs_exc.c ucs_factory.c ucs_fsm.c ucs_gpio.c ucs_i2c.c ucs_inic.c ucs_inic_res.c ucs_jobs.c ucs_lldpool.c ucs_message.c ucs_mgr.c ucs_misc.c ucs_net.c ucs_nodedis.c ucs_nodeobserver.c ucs_nsm.c ucs_obs.c ucs_pmchannel.c ucs_pmcmd.c ucs_pmevent.c ucs_pmfifo.c ucs_pmfifos.c ucs_pmp.c ucs_pool.c ucs_prog.c ucs_rsm.c ucs_rtm.c ucs_scheduler.c ucs_segmentation.c ucs_smm.c ucs_sys_diag.c ucs_telqueue.c ucs_timer.c ucs_transceiver.c ucs_xrm.c ucs_xrmpool.c ucs_xrm_res.c)

    # Expose Library Properties
    SET_TARGET_PROPERTIES(ucs2-lib PROPERTIES OUTPUT_NAME ucs2net)

    # Library dependencies from PKG_REQUIRED_LIST
    TARGET_LINK_LIBRARIES(ucs2-lib ${link_libraries})

    # Define properties to expose when others use this target
    TARGET_INCLUDE_DIRECTORIES(ucs2-lib 
       PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/../inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../cfg
        ${CMAKE_CURRENT_SOURCE_DIR}/ucs-xml
    )

