/*
 * Unicens Integration Helper Component
 *
 * Copyright (C) 2017 Microchip Technology Germany II GmbH & Co. KG
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * You may also obtain this software under a propriety license from Microchip.
 * Please contact Microchip for further information.
 *
 */
#include <assert.h>
#include "ucs_interface.h"

/************************************************************************/
/* Private Definitions and variables                                    */
/************************************************************************/
#define MAGIC (0xA144BEAF)

/************************************************************************/
/* Private Function Prototypes                                          */
/************************************************************************/
static bool EnqueueCommand(UCSI_Data_t *my, UnicensCmdEntry_t *cmd);
static void OnCommandExecuted(UCSI_Data_t *my, UnicensCmd_t cmd);
static void RB_Init(RB_t *rb, uint16_t amountOfEntries, uint32_t sizeOfEntry, uint8_t *workingBuffer);
static void *RB_GetReadPtr(RB_t *rb);
static void RB_PopReadPtr(RB_t *rb);
static void *RB_GetWritePtr(RB_t *rb);
static void RB_PopWritePtr(RB_t *rb);
static uint16_t OnUnicensGetTime(void *user_ptr);
static void OnUnicensService( void *user_ptr );
static void OnUnicensError( Ucs_Error_t error_code, void *user_ptr );
static void OnUnicensAppTimer( uint16_t timeout, void *user_ptr );
static void OnUnicensDebugErrorMsg(Msg_MostTel_t *m, void *user_ptr);
static void OnLldCtrlStart( Ucs_Lld_Api_t* api_ptr, void *inst_ptr, void *lld_user_ptr );
static void OnLldCtrlStop( void *lld_user_ptr );
static void OnLldCtrlRxMsgAvailable( void *lld_user_ptr );
static void OnLldCtrlTxTransmitC( Ucs_Lld_TxMsg_t *msg_ptr, void *lld_user_ptr );
static void OnUnicensRoutingResult(Ucs_Rm_Route_t* route_ptr, Ucs_Rm_RouteInfos_t route_infos, void *user_ptr);
static void OnUnicensMostPortStatus(uint16_t most_port_handle,
    Ucs_Most_PortAvail_t availability, Ucs_Most_PortAvailInfo_t avail_info,
    uint16_t free_streaming_bw, void* user_ptr);
static void OnUnicensDebugXrmResources(Ucs_Xrm_ResourceType_t resource_type,
    Ucs_Xrm_ResObject_t *resource_ptr, Ucs_Xrm_ResourceInfos_t resource_infos,
    Ucs_Rm_EndPoint_t *endpoint_inst_ptr, void *user_ptr);
static void OnUcsInitResult(Ucs_InitResult_t result, void *user_ptr);
static void OnUcsStopResult(Ucs_StdResult_t result, void *user_ptr);
static void OnUcsMgrReport(Ucs_MgrReport_t code, uint16_t node_address, Ucs_Rm_Node_t *node_ptr, void *user_ptr);
static void OnUcsNsRun(Ucs_Rm_Node_t * node_ptr, Ucs_Ns_ResultCode_t result, void *ucs_user_ptr);

/************************************************************************/
/* Public Function Implementations                                      */
/************************************************************************/
void UCSI_Init(UCSI_Data_t *my, void *pTag)
{
    Ucs_Return_t result;
    assert(NULL != my);
    memset(my, 0, sizeof(UCSI_Data_t));
    my->magic = MAGIC;
    my->tag = pTag;
    my->UNICENS = Ucs_CreateInstance();
    if (NULL == my->UNICENS)
    {
        UCSI_CB_OnUserMessage(my->tag, "Can not instance a new version of UNICENS, "\
            "increase UCS_NUM_INSTANCES define", 0);
        assert(false);
        return;
    }
    result = Ucs_SetDefaultConfig(&my->uniInitData);
    if(UCS_RET_SUCCESS != result)
    {
        UCSI_CB_OnUserMessage(my->tag, "Can not set default values to Unicens config (result=0x%X)", 1, result);
        assert(false);
        return;
    }
    my->uniInitData.user_ptr = my;
    my->uniInitData.mgr.report_fptr = OnUcsMgrReport;

    my->uniInitData.general.inic_watchdog_enabled = ENABLE_INIC_WATCHDOG;
    my->uniInitData.general.get_tick_count_fptr = &OnUnicensGetTime;
    my->uniInitData.general.request_service_fptr = &OnUnicensService;
    my->uniInitData.general.error_fptr = &OnUnicensError;
    my->uniInitData.general.set_application_timer_fptr = &OnUnicensAppTimer;
    my->uniInitData.general.debug_error_msg_fptr = &OnUnicensDebugErrorMsg;
    my->uniInitData.ams.enabled = ENABLE_AMS_LIB;

    my->uniInitData.lld.lld_user_ptr = my;
    my->uniInitData.lld.start_fptr =  &OnLldCtrlStart;
    my->uniInitData.lld.stop_fptr = &OnLldCtrlStop;
    my->uniInitData.lld.rx_available_fptr = &OnLldCtrlRxMsgAvailable;
    my->uniInitData.lld.tx_transmit_fptr = &OnLldCtrlTxTransmitC;

    my->uniInitData.rm.report_fptr = &OnUnicensRoutingResult;
    my->uniInitData.rm.xrm.most_port_status_fptr = &OnUnicensMostPortStatus;
    my->uniInitData.rm.debug_resource_status_fptr = &OnUnicensDebugXrmResources;

    RB_Init(&my->rb, CMD_QUEUE_LEN, sizeof(UnicensCmdEntry_t), my->rbBuf);
}

bool UCSI_NewConfig(UCSI_Data_t *my, UcsXmlVal_t *ucsConfig) {

    UnicensCmdEntry_t *e;
    assert(MAGIC == my->magic);
    if (my->initialized)
    {
        e = (UnicensCmdEntry_t *)RB_GetWritePtr(&my->rb);
        if (NULL == e) return false;
        e->cmd = UnicensCmd_Stop;
        RB_PopWritePtr(&my->rb);
    }
    my->uniInitData.mgr.packet_bw = ucsConfig->packetBw;
    my->uniInitData.mgr.routes_list_ptr = ucsConfig->pRoutes;
    my->uniInitData.mgr.routes_list_size = ucsConfig->routesSize;
    my->uniInitData.mgr.nodes_list_ptr = ucsConfig->pNod;
    my->uniInitData.mgr.nodes_list_size = ucsConfig->nodSize;
    my->uniInitData.mgr.enabled = true;
    e = (UnicensCmdEntry_t *)RB_GetWritePtr(&my->rb);
    if (NULL == e) return false;
    e->cmd =  UnicensCmd_Init;
    e->val.Init.init_ptr = &my->uniInitData;
    RB_PopWritePtr(&my->rb);
    UCSI_CB_OnServiceRequired(my->tag);
    return true;
}

bool UCSI_ProcessRxData(UCSI_Data_t *my, const uint8_t *pBuffer, uint16_t len)
{
    assert(MAGIC == my->magic);
    if (NULL == my->uniLld || NULL == my->uniLldHPtr) return false;
    Ucs_Lld_RxMsg_t *msg = NULL;
    msg = my->uniLld->rx_allocate_fptr(my->uniLldHPtr, len);
    if (NULL == msg)
    {
        //This may happen by definition, OnLldCtrlRxMsgAvailable()
        //will be called, once buffers are available again
        return false;
    }
    msg->data_size = len;
    memcpy(msg->data_ptr, pBuffer, len);
    my->uniLld->rx_receive_fptr(my->uniLldHPtr, msg);
    return true;
}


void UCSI_Service(UCSI_Data_t *my) {
    UnicensCmdEntry_t *e;
    bool popEntry = true; //Set to false in specific case, where function will callback asynchrony.
    assert(MAGIC == my->magic);
    if (NULL != my->UNICENS && my->triggerService) {
        my->triggerService = false;
        Ucs_Service(my->UNICENS);
    }
    if (NULL != my->currentCmd) return;
    my->currentCmd = e = (UnicensCmdEntry_t *)RB_GetReadPtr(&my->rb);
    if (NULL == e) return;
    switch (e->cmd) {
        case UnicensCmd_Init:
            if (UCS_RET_SUCCESS == Ucs_Init(my->UNICENS, e->val.Init.init_ptr, OnUcsInitResult))
                popEntry = false;
            else
                UCSI_CB_OnUserMessage(my->tag, "Ucs_Init failed", 0);
            break;
        case UnicensCmd_Stop:
            if (UCS_RET_SUCCESS == Ucs_Stop(my->UNICENS, OnUcsStopResult))
                popEntry = false;
            else
                UCSI_CB_OnUserMessage(my->tag, "Ucs_Stop failed", 0);
            break;
        case UnicensCmd_RmSetRoute:
            if (UCS_RET_SUCCESS != Ucs_Rm_SetRouteActive(my->UNICENS, e->val.RmSetRoute.routePtr, e->val.RmSetRoute.isActive))
                UCSI_CB_OnUserMessage(my->tag, "Ucs_Rm_SetRouteActive failed", 0);
            break;
        case UnicensCmd_NsRun:
            if (UCS_RET_SUCCESS != Ucs_Ns_Run(my->UNICENS, e->val.NsRun.node_ptr, OnUcsNsRun))
                UCSI_CB_OnUserMessage(my->tag, "Ucs_Ns_Run failed", 0);
            break;
        default:
            assert(false);
            break;
    }
    if (popEntry)
    {
        my->currentCmd = NULL;
        RB_PopReadPtr(&my->rb);
    }
}

void UCSI_Timeout(UCSI_Data_t *my)
{
    assert(MAGIC == my->magic);
    if (NULL == my->UNICENS) return;
    Ucs_ReportTimeout(my->UNICENS);
}

/************************************************************************/
/* Private Functions                                                    */
/************************************************************************/
static bool EnqueueCommand(UCSI_Data_t *my, UnicensCmdEntry_t *cmd)
{
    UnicensCmdEntry_t *e;
    if (NULL == my || NULL == cmd)
    {
        assert(false);
        return false;
    }
    e = RB_GetWritePtr(&my->rb);
    if (NULL == e)
    {
        UCSI_CB_OnUserMessage(my->tag, "Could not enqueue command. Increase CMD_QUEUE_LEN define", 0);
        return false;
    }
    memcpy(e, cmd, sizeof(UnicensCmdEntry_t));
    RB_PopWritePtr(&my->rb);
    UCSI_CB_OnServiceRequired(my->tag);
    return true;
}

static void OnCommandExecuted(UCSI_Data_t *my, UnicensCmd_t cmd)
{
    if (NULL == my)
    {
        assert(false);
        return;
    }
    if (NULL == my->currentCmd)
    {
        UCSI_CB_OnUserMessage(my->tag, "OnUniCommandExecuted was called, but no "\
            "command is in queue", 0);
        assert(false);
        return;
    }
    if (my->currentCmd->cmd != cmd)
    {
        UCSI_CB_OnUserMessage(my->tag, "OnUniCommandExecuted was called with "\
            "wrong command (Expected=0x%X, Got=0x%X", 2, my->currentCmd->cmd, cmd);
        assert(false);
        return;
    }
    my->currentCmd = NULL;
    RB_PopReadPtr(&my->rb);
}

static void RB_Init(RB_t *rb, uint16_t amountOfEntries, uint32_t sizeOfEntry, uint8_t *workingBuffer)
{
    assert(NULL != rb);
    assert(NULL != workingBuffer);
    rb->dataQueue = workingBuffer;
    rb->pRx = rb->dataQueue;
    rb->pTx = rb->dataQueue;
    rb->amountOfEntries = amountOfEntries;
    rb->sizeOfEntry = sizeOfEntry;
    rb->rxPos = 0;
    rb->txPos = 0;
}

static void *RB_GetReadPtr(RB_t *rb)
{
    assert(NULL != rb);
    assert(0 != rb->dataQueue);
    if (rb->txPos - rb->rxPos > 0)
    return (void *)rb->pRx;
    return NULL;
}

static void RB_PopReadPtr(RB_t *rb)
{
    assert(NULL != rb);
    assert(0 != rb->dataQueue);

    rb->pRx += rb->sizeOfEntry;
    if (rb->pRx >= rb->dataQueue + ( rb->amountOfEntries * rb->sizeOfEntry))
    rb->pRx = rb->dataQueue;
    ++rb->rxPos;
    assert(rb->txPos >= rb->rxPos);
}

static void *RB_GetWritePtr(RB_t *rb)
{
    assert(NULL != rb);
    assert(0 != rb->dataQueue);
    if (rb->txPos - rb->rxPos < rb->amountOfEntries)
    return (void *)rb->pTx;
    return NULL;
}

static void RB_PopWritePtr(RB_t *rb)
{
    assert(NULL != rb);
    assert(0 != rb->dataQueue);
    rb->pTx += rb->sizeOfEntry;
    if (rb->pTx >= rb->dataQueue + ( rb->amountOfEntries * rb->sizeOfEntry))
    rb->pTx = rb->dataQueue;
    ++rb->txPos;
    assert(rb->txPos >= rb->rxPos);
}

static uint16_t OnUnicensGetTime(void *user_ptr)
{
    UCSI_Data_t *my = (UCSI_Data_t *)user_ptr;
    assert(MAGIC == my->magic);
    return UCSI_CB_OnGetTime(my->tag);
}

static void OnUnicensService( void *user_ptr )
{
    UCSI_Data_t *my = (UCSI_Data_t *)user_ptr;
    assert(MAGIC == my->magic);
    my->triggerService = true;
    UCSI_CB_OnServiceRequired(my->tag);
}

static void OnUnicensError( Ucs_Error_t error_code, void *user_ptr )
{
    error_code = error_code;
    UCSI_Data_t *my = (UCSI_Data_t *)user_ptr;
    assert(MAGIC == my->magic);
    UCSI_CB_OnUserMessage(my->tag, "Unicens general error, code=0x%X", 1, error_code);
}

static void OnUnicensAppTimer( uint16_t timeout, void *user_ptr )
{
    UCSI_Data_t *my = (UCSI_Data_t *)user_ptr;
    assert(MAGIC == my->magic);
    UCSI_CB_OnSetServiceTimer(my->tag, timeout);
}

static void OnUnicensDebugErrorMsg(Msg_MostTel_t *m, void *user_ptr)
{
    UCSI_Data_t *my = (UCSI_Data_t *)user_ptr;
    assert(MAGIC == my->magic);
    UCSI_CB_OnMostError(my->tag, m->source_addr, m->id.fblock_id, m->id.instance_id,
        m->id.function_id, m->id.op_type, m->tel.tel_data_ptr, m->tel.tel_len);
}

static void OnLldCtrlStart( Ucs_Lld_Api_t* api_ptr, void *inst_ptr, void *lld_user_ptr )
{
    UCSI_Data_t *my = (UCSI_Data_t *)lld_user_ptr;
    assert(MAGIC == my->magic);
    my->uniLld = api_ptr;
    my->uniLldHPtr = inst_ptr;
}

static void OnLldCtrlStop( void *lld_user_ptr )
{
    UCSI_Data_t *my = (UCSI_Data_t *)lld_user_ptr;
    assert(MAGIC == my->magic);
    my->uniLld = NULL;
    my->uniLldHPtr = NULL;
}

static void OnLldCtrlRxMsgAvailable( void *lld_user_ptr )
{
    UCSI_Data_t *my = (UCSI_Data_t *)lld_user_ptr;
    assert(MAGIC == my->magic);
    UCSI_CB_OnServiceRequired(my->tag);
}

static void OnLldCtrlTxTransmitC( Ucs_Lld_TxMsg_t *msg_ptr, void *lld_user_ptr )
{
    UCSI_Data_t *my = (UCSI_Data_t *)lld_user_ptr;
    assert(MAGIC == my->magic);
    uint8_t buffer[BOARD_PMS_TX_SIZE];
    uint32_t bufferPos = 0;
    Ucs_Mem_Buffer_t * buf_ptr;
    if (NULL == msg_ptr || NULL == my || NULL == my->uniLld || NULL == my->uniLldHPtr)
    {
        assert(false);
        return;
    }
    for (buf_ptr = msg_ptr->memory_ptr; buf_ptr != NULL; buf_ptr = buf_ptr->next_buffer_ptr)
    {
        if (buf_ptr->data_size + bufferPos > sizeof(buffer))
        {
            UCSI_CB_OnUserMessage(my->tag, "TX buffer is too small, increase " \
                "BOARD_PMS_TX_SIZE define (%lu > %lu)", 2, buf_ptr->data_size + bufferPos, sizeof(buffer));
            my->uniLld->tx_release_fptr(my->uniLldHPtr, msg_ptr);
            return;
        }
        memcpy(&buffer[bufferPos], buf_ptr->data_ptr, buf_ptr->data_size);
        bufferPos += buf_ptr->data_size;
    }
    assert(bufferPos == msg_ptr->memory_ptr->total_size);
    my->uniLld->tx_release_fptr(my->uniLldHPtr, msg_ptr);
    UCSI_CB_SendMostMessage(my->tag, buffer, bufferPos);
}

static void OnUnicensRoutingResult(Ucs_Rm_Route_t* route_ptr, Ucs_Rm_RouteInfos_t route_infos, void *user_ptr)
{
    //TODO: implement
    route_ptr = route_ptr;
    route_infos = route_infos;
    user_ptr = user_ptr;
}

static void OnUnicensMostPortStatus(uint16_t most_port_handle,
    Ucs_Most_PortAvail_t availability, Ucs_Most_PortAvailInfo_t avail_info,
    uint16_t free_streaming_bw, void* user_ptr)
{
    //TODO: implement
    most_port_handle = most_port_handle;
    availability = availability;
    avail_info = avail_info;
    free_streaming_bw = free_streaming_bw;
    user_ptr = user_ptr;
}

static void OnUnicensDebugXrmResources(Ucs_Xrm_ResourceType_t resource_type,
    Ucs_Xrm_ResObject_t *resource_ptr, Ucs_Xrm_ResourceInfos_t resource_infos,
    Ucs_Rm_EndPoint_t *endpoint_inst_ptr, void *user_ptr)
{
#ifndef DEBUG_XRM
    resource_type = resource_type;
    resource_ptr = resource_ptr;
    resource_infos = resource_infos;
    endpoint_inst_ptr = endpoint_inst_ptr;
    user_ptr = user_ptr;
#else
    endpoint_inst_ptr = endpoint_inst_ptr;
    UCSI_Data_t *my = (UCSI_Data_t *)user_ptr;
    assert(MAGIC == my->magic);
    if (NULL == resource_ptr) return;
    char *msg = NULL;
    uint16_t adr = 0xFFFF;
    if (endpoint_inst_ptr && endpoint_inst_ptr->node_obj_ptr &&
            endpoint_inst_ptr->node_obj_ptr->signature_ptr)
        adr = endpoint_inst_ptr->node_obj_ptr->signature_ptr->node_address;
    switch (resource_infos)
    {
        case UCS_XRM_INFOS_BUILT:
        msg = (char *)"has been built";
        break;
        case UCS_XRM_INFOS_DESTROYED:
        msg = (char *)"has been destroyed";
        break;
        case UCS_XRM_INFOS_ERR_BUILT:
        msg = (char *)"cannot be built";
        break;
        default:
        msg = (char *)"cannot be destroyed";
        break;
    }
    switch(resource_type)
    {
        case UCS_XRM_RC_TYPE_MOST_SOCKET:
        {
            Ucs_Xrm_MostSocket_t *ms = (Ucs_Xrm_MostSocket_t *)resource_ptr;
            assert(ms->resource_type == resource_type);
            UCSI_CB_OnUserMessage(my->tag, "Xrm-Debug (0x%03X): MOST socket %s, handle=%04X, "\
                "direction=%d, type=%d, bandwidth=%d", 6, adr, msg, ms->most_port_handle,
                ms->direction, ms->data_type, ms->bandwidth);
            break;
        }
        case UCS_XRM_RC_TYPE_MLB_PORT:
        {
            Ucs_Xrm_MlbPort_t *m = (Ucs_Xrm_MlbPort_t *)resource_ptr;
            assert(m->resource_type == resource_type);
            UCSI_CB_OnUserMessage(my->tag, "Xrm-Debug (0x%03X): MLB port %s, index=%d, clock=%d", 4,
                adr, msg, m->index, m->clock_config);
            break;
        }
        case UCS_XRM_RC_TYPE_MLB_SOCKET:
        {
            Ucs_Xrm_MlbSocket_t *m = (Ucs_Xrm_MlbSocket_t *)resource_ptr;
            assert(m->resource_type == resource_type);
            UCSI_CB_OnUserMessage(my->tag, "Xrm-Debug (0x%03X): MLB socket %s, direction=%d, type=%d,"\
                " bandwidth=%d, channel=%d", 6, adr, msg, m->direction, m->data_type,
                m->bandwidth, m->channel_address);
            break;
        }
        case UCS_XRM_RC_TYPE_USB_PORT:
        {
            Ucs_Xrm_UsbPort_t *m = (Ucs_Xrm_UsbPort_t *)resource_ptr;
            assert(m->resource_type == resource_type);
            UCSI_CB_OnUserMessage(my->tag, "Xrm-Debug (0x%03X): USB port %s, in-cnt=%d, out-cnt=%d", 4,
                adr, msg, m->streaming_if_ep_in_count, m->streaming_if_ep_out_count);
            break;
        }
        case UCS_XRM_RC_TYPE_USB_SOCKET:
        {
            Ucs_Xrm_UsbSocket_t *m = (Ucs_Xrm_UsbSocket_t *)resource_ptr;
            assert(m->resource_type == resource_type);
            UCSI_CB_OnUserMessage(my->tag, "Xrm-Debug (0x%03X): USB socket %s, direction=%d, type=%d," \
                " ep-addr=%02X, frames=%d", 6, adr, msg, m->direction, m->data_type,
                m->end_point_addr, m->frames_per_transfer);
            break;
        }
        case UCS_XRM_RC_TYPE_STRM_PORT:
        {
            Ucs_Xrm_StrmPort_t *m = (Ucs_Xrm_StrmPort_t *)resource_ptr;
            assert(m->resource_type == resource_type);
            UCSI_CB_OnUserMessage(my->tag, "Xrm-Debug (0x%03X): I2S port %s, index=%d, clock=%d, "\
                "align=%d", 5, adr, msg, m->index, m->clock_config, m->data_alignment);
            break;
        }
        case UCS_XRM_RC_TYPE_STRM_SOCKET:
        {
            Ucs_Xrm_StrmSocket_t *m = (Ucs_Xrm_StrmSocket_t *)resource_ptr;
            assert(m->resource_type == resource_type);
            UCSI_CB_OnUserMessage(my->tag, "Xrm-Debug (0x%03X): I2S socket %s, direction=%d, type=%d"\
                ", bandwidth=%d, pin=%d", 6, adr, msg, m->direction, m->data_type,
                m->bandwidth, m->stream_pin_id);
            break;
        }
        case UCS_XRM_RC_TYPE_SYNC_CON:
        {
            Ucs_Xrm_SyncCon_t *m = (Ucs_Xrm_SyncCon_t *)resource_ptr;
            assert(m->resource_type == resource_type);
            UCSI_CB_OnUserMessage(my->tag, "Xrm-Debug (0x%03X): Sync connection %s, mute=%d, "\
                "offset=%d", 4, adr, msg, m->mute_mode, m->offset);
            break;
        }
        case UCS_XRM_RC_TYPE_COMBINER:
        {
            Ucs_Xrm_Combiner_t *m = (Ucs_Xrm_Combiner_t *)resource_ptr;
            assert(m->resource_type == resource_type);
            UCSI_CB_OnUserMessage(my->tag, "Xrm-Debug (0x%03X): Combiner %s, bytes per frame=%d",
                3, adr, msg, m->bytes_per_frame);
            break;
        }
        case UCS_XRM_RC_TYPE_SPLITTER:
        {
            Ucs_Xrm_Splitter_t *m = (Ucs_Xrm_Splitter_t *)resource_ptr;
            assert(m->resource_type == resource_type);
            UCSI_CB_OnUserMessage(my->tag, "Xrm-Debug (0x%03X): Splitter %s, bytes per frame=%d",
                3, adr, msg, m->bytes_per_frame);
            break;
        }
        case UCS_XRM_RC_TYPE_AVP_CON:
        {
            Ucs_Xrm_AvpCon_t *m = (Ucs_Xrm_AvpCon_t *)resource_ptr;
            assert(m->resource_type == resource_type);
            UCSI_CB_OnUserMessage(my->tag, "Xrm-Debug (0x%03X): Isoc-AVP connection %s, packetSize=%d",
                3, adr, msg, m->isoc_packet_size);
            break;
        }
        default:
        UCSI_CB_OnUserMessage(my->tag, "Xrm-Debug (0x%03X): Unknown type=%d %s", 3 , adr, resource_type, msg);
    }
#endif
}

static void OnUcsInitResult(Ucs_InitResult_t result, void *user_ptr)
{
    UnicensCmdEntry_t e;
    UCSI_Data_t *my = (UCSI_Data_t *)user_ptr;
    assert(MAGIC == my->magic);
    my->initialized = (UCS_INIT_RES_SUCCESS == result);
    OnCommandExecuted(my, UnicensCmd_Init);
    if (!my->initialized)
    {
        UCSI_CB_OnUserMessage(my->tag, "UcsInitResult reported error (0x%X), restarting...", 1, result);
        e.cmd = UnicensCmd_Init;
        e.val.Init.init_ptr = &my->uniInitData;
        EnqueueCommand(my, &e);
    }
}

static void OnUcsStopResult(Ucs_StdResult_t result, void *user_ptr)
{
    result = result; //TODO: check error case
    UCSI_Data_t *my = (UCSI_Data_t *)user_ptr;
    assert(MAGIC == my->magic);
    my->initialized = false;
    OnCommandExecuted(my, UnicensCmd_Stop);
    UCSI_CB_OnStop(my->tag);
}

static void OnUcsMgrReport(Ucs_MgrReport_t code, uint16_t node_address, Ucs_Rm_Node_t *node_ptr, void *user_ptr)
{
    UCSI_Data_t *my = (UCSI_Data_t *)user_ptr;
    assert(MAGIC == my->magic);
    if (node_ptr && node_ptr->script_list_ptr && node_ptr->script_list_size)
    {
        UnicensCmdEntry_t e;
        e.cmd = UnicensCmd_NsRun;
        e.val.NsRun.node_ptr = node_ptr;
        EnqueueCommand(my, &e);
    }
    UCSI_CB_OnMgrReport(my->tag, code, node_address, node_ptr);
}

static void OnUcsNsRun(Ucs_Rm_Node_t * node_ptr, Ucs_Ns_ResultCode_t result, void *ucs_user_ptr)
{
#ifndef DEBUG_XRM
    node_ptr = node_ptr;
    result = result;
    ucs_user_ptr;
#else
    UCSI_Data_t *my = (UCSI_Data_t *)ucs_user_ptr;
    assert(MAGIC == my->magic);
    UCSI_CB_OnUserMessage(my->tag, "OnUcsNsRun (%03X): script executed %s",
        2, node_ptr->signature_ptr->node_address,
        (UCS_NS_RES_SUCCESS == result ? "succeeded" : "false"));
#endif
}

/*----------------------------------------
 * Debug Message output from Unicens stack:
 *----------------------------------------
 */
#if defined(UCS_TR_ERROR) || defined(UCS_TR_INFO)
#include <stdio.h>
#define TRACE_BUFFER_SZ 200
void App_TraceError(void *ucs_user_ptr, const char module_str[], const char entry_str[], uint16_t vargs_cnt, ...)
{
    void *tag = NULL;
    UCSI_Data_t *my = (UCSI_Data_t *)ucs_user_ptr;
    if (my)
    {
        assert(MAGIC == my->magic);
        tag = my->tag;
    }
    char outbuf[TRACE_BUFFER_SZ];
    va_list argptr;
    va_start(argptr, vargs_cnt);
    vsprintf(outbuf, entry_str, argptr);
    va_end(argptr);
    UCSI_CB_OnUserMessage(tag, "Error | %s | %s", 2, module_str, outbuf);
}
void App_TraceInfo(void *ucs_user_ptr, const char module_str[], const char entry_str[], uint16_t vargs_cnt, ...)
{
    void *tag = NULL;
    UCSI_Data_t *my = (UCSI_Data_t *)ucs_user_ptr;
    if (my)
    {
        assert(MAGIC == my->magic);
        tag = my->tag;
    }
    char outbuf[TRACE_BUFFER_SZ];
    va_list argptr;
    va_start(argptr, vargs_cnt);
    vsprintf(outbuf, entry_str, argptr);
    va_end(argptr);
    UCSI_CB_OnUserMessage(tag, "Info | %s | %s", 2, module_str, outbuf);
}
#endif
