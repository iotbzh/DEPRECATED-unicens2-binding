
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
ADD_LIBRARY(ucs2-lib STATIC
 unicens/src/ucs_alm.c
 unicens/src/ucs_amd.c
 unicens/src/ucs_ams.c
 unicens/src/ucs_amsmessage.c
 unicens/src/ucs_amspool.c
 unicens/src/ucs_amtp.c
 unicens/src/ucs_attach.c
 unicens/src/ucs_base.c
 unicens/src/ucs_bc_diag.c
 unicens/src/ucs_class.c
 unicens/src/ucs_cmd.c
 unicens/src/ucs_dec.c
 unicens/src/ucs_dl.c
 unicens/src/ucs_eh.c
 unicens/src/ucs_encoder.c
 unicens/src/ucs_epm.c
 unicens/src/ucs_exc.c
 unicens/src/ucs_factory.c
 unicens/src/ucs_fsm.c
 unicens/src/ucs_gpio.c
 unicens/src/ucs_i2c.c
 unicens/src/ucs_inic.c
 unicens/src/ucs_inic_res.c
 unicens/src/ucs_jobs.c
 unicens/src/ucs_lldpool.c
 unicens/src/ucs_message.c
 unicens/src/ucs_mgr.c
 unicens/src/ucs_misc.c
 unicens/src/ucs_net.c
 unicens/src/ucs_nodedis.c
 unicens/src/ucs_nodeobserver.c
 unicens/src/ucs_nsm.c
 unicens/src/ucs_obs.c
 unicens/src/ucs_pmchannel.c
 unicens/src/ucs_pmcmd.c
 unicens/src/ucs_pmevent.c
 unicens/src/ucs_pmfifo.c
 unicens/src/ucs_pmfifos.c
 unicens/src/ucs_pmp.c
 unicens/src/ucs_pool.c
 unicens/src/ucs_prog.c
 unicens/src/ucs_rsm.c
 unicens/src/ucs_rtm.c
 unicens/src/ucs_scheduler.c
 unicens/src/ucs_segmentation.c
 unicens/src/ucs_smm.c
 unicens/src/ucs_sys_diag.c
 unicens/src/ucs_telqueue.c
 unicens/src/ucs_timer.c
 unicens/src/ucs_transceiver.c
 unicens/src/ucs_xrm.c
 unicens/src/ucs_xrmpool.c
 unicens/src/ucs_xrm_res.c)

    # Expose Library Properties
    SET_TARGET_PROPERTIES(ucs2-lib PROPERTIES OUTPUT_NAME ucs2net)

    # Library dependencies from PKG_REQUIRED_LIST
    TARGET_LINK_LIBRARIES(ucs2-lib ${link_libraries})

    # Define properties to expose when others use this target
    TARGET_INCLUDE_DIRECTORIES(ucs2-lib
       PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/unicens/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/cfg_agl
    )

