/*------------------------------------------------------------------------------------------------*/
/* UNICENS V2.1.0-3491                                                                            */
/* Copyright (c) 2017 Microchip Technology Germany II GmbH & Co. KG.                              */
/*                                                                                                */
/* This program is free software: you can redistribute it and/or modify                           */
/* it under the terms of the GNU General Public License as published by                           */
/* the Free Software Foundation, either version 2 of the License, or                              */
/* (at your option) any later version.                                                            */
/*                                                                                                */
/* This program is distributed in the hope that it will be useful,                                */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of                                 */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                                  */
/* GNU General Public License for more details.                                                   */
/*                                                                                                */
/* You should have received a copy of the GNU General Public License                              */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>.                          */
/*                                                                                                */
/* You may also obtain this software under a propriety license from Microchip.                    */
/* Please contact Microchip for further information.                                              */
/*------------------------------------------------------------------------------------------------*/

/*!
 * \file
 * \brief Implementation of Application Message Distributor
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup  G_AMD
 * @{
 */

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_amd.h"
#include "ucs_misc.h"

/*! \brief Priority of the Application Message Distribution */
static const uint8_t AMD_SRV_PRIO           = 248U; /* parasoft-suppress  MISRA2004-8_7 "configuration property" */
/*! \brief Event which starts the Rx message distribution */
static const Srv_Event_t AMD_EV_NOTIFY_RX   = 1U;
/*! \brief Event triggers notification of messages in tx_notify_queue */
static const Srv_Event_t AMD_EV_NOTIFY_TX   = 2U;
/*! \brief FBlockID of FBlock NetBlock */
static const uint8_t AMD_FB_NETBLOCK        = 1U;
/*! \brief FBlockID of FBlock NetworkMaster */
static const uint8_t AMD_FB_NETWORKMASTER   = 2U;

/*------------------------------------------------------------------------------------------------*/
/* Internal prototypes                                                                            */
/*------------------------------------------------------------------------------------------------*/
static void Amd_Service(void *self);
static void Amd_OnAmsComplete(void* self, Ucs_AmsRx_Msg_t* msg_ptr);
static void Amd_OnEvent(void *self, void *error_code_ptr);
static void Amd_OnTerminateEvent(void *self, void *error_code_ptr);
static void Amd_RxFlush(CAmd *self, CDlList *list_ptr);
#ifdef AMD_TX_DISTRIB
static bool Amd_TxIsRcmMsg(Ucs_AmsTx_Msg_t *msg_ptr);
static bool Amd_TxReceiveInternal(CAmd *self, Ucs_AmsTx_Msg_t *msg_ptr);
static void Amd_TxProcessNotifyQueue(CAmd *self);
#endif

/*------------------------------------------------------------------------------------------------*/
/* Initialization                                                                                 */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Constructor of class CAmd
 *  \param self     The instance
 *  \param base_ptr Reference to base services
 *  \param ams_ptr  Reference to the AMS
 */
void Amd_Ctor(CAmd *self, CBase *base_ptr, CAms *ams_ptr)
{
    MISC_MEM_SET((void *)self, 0, sizeof(*self));                 /* reset members to "0" */

    self->base_ptr = base_ptr;
    self->ams_ptr = ams_ptr;

    self->started = false;
    Srv_Ctor(&self->service, AMD_SRV_PRIO, self, &Amd_Service);   /* register service */
    (void)Scd_AddService(&self->base_ptr->scd, &self->service);

    Dl_Ctor(&self->pre_queue, self->base_ptr->ucs_user_ptr);       /* init preprocessor queue */
    Dl_Ctor(&self->rx_queue, self->base_ptr->ucs_user_ptr);        /* init Rx queue */
                                                                  /* register event observer */
    Mobs_Ctor(&self->event_observer, self, EH_E_INIT_SUCCEEDED, &Amd_OnEvent);
    Eh_AddObsrvInternalEvent(&self->base_ptr->eh, &self->event_observer);
                                                                  /* register termination events */
    Mobs_Ctor(&self->terminate_observer, self, EH_M_TERMINATION_EVENTS, &Amd_OnTerminateEvent);
    Eh_AddObsrvInternalEvent(&self->base_ptr->eh, &self->terminate_observer);

    Ams_RxAssignReceiver(self->ams_ptr, &Amd_OnAmsComplete, self);
}


#ifdef AMD_TX_DISTRIB
/*! \brief Constructor of class CAmd
 *  \param self     The instance
 *  \param base_ptr Reference to base services
 *  \param ams_ptr  Reference to the AMS
 *  \param pool_ptr Reference to the AMS message pool
 *  \param inic_ptr Reference to the INIC
 *  \param net_ptr  Reference to the network management
 */
void Amd_Ctor(CAmd *self, CBase *base_ptr, CAms *ams_ptr, CAmsMsgPool *pool_ptr, CInic *inic_ptr, CNetworkManagement *net_ptr)
{
    MISC_MEM_SET((void *)self, 0, sizeof(*self));                 /* reset members to "0" */

    self->base_ptr = base_ptr;
    self->ams_ptr = ams_ptr;
    self->pool_ptr = pool_ptr;
    self->inic_ptr = inic_ptr;
    self->net_ptr = net_ptr;

    self->started = false;
    Srv_Ctor(&self->service, AMD_SRV_PRIO, self, &Amd_Service);   /* register service */
    (void)Scd_AddService(&self->base_ptr->scd, &self->service);

    Dl_Ctor(&self->tx_notify_queue, self->base_ptr->ucs_inst_id); /* init queues */
    Dl_Ctor(&self->pre_queue, self->base_ptr->ucs_inst_id);       /* init preprocessor queue */
    Dl_Ctor(&self->rx_queue, self->base_ptr->ucs_inst_id);        /* init Rx queue */
                                                                  /* register event observer */
    Mobs_Ctor(&self->event_observer, self, EH_E_INIT_SUCCEEDED, &Amd_OnEvent);
    Eh_AddObsrvInternalEvent(&self->base_ptr->eh, &self->event_observer);
                                                                  /* register termination events */
    Mobs_Ctor(&self->terminate_observer, self, EH_M_TERMINATION_EVENTS, &Amd_OnTerminateEvent);
    Eh_AddObsrvInternalEvent(&self->base_ptr->eh, &self->terminate_observer);

    Ams_RxAssignReceiver(self->ams_ptr, &Amd_OnAmsComplete, self);
    Ams_TxAssignTrcvSelector(self->ams_ptr, &Amd_TxIsRcmMsg);
}
#endif

/*! \brief   Assigns a pre-processor callback function for Rx messages
 *  \details This function must be called during initialization time.
 *           The AMS shall not already run.
 *  \param   self          The instance
 *  \param   callback_fptr Reference to the callback function
 *  \param   inst_ptr      Reference to the pre-processor
 */
void Amd_AssignPreprocessor(CAmd *self, Amd_RxMsgCompleteCb_t callback_fptr, void *inst_ptr)
{
    if (callback_fptr != NULL)
    {
        self->preprocess_fptr = callback_fptr;
        self->preprocess_inst_ptr = inst_ptr;

        self->first_receive_fptr = callback_fptr;
        self->first_receive_inst_ptr = inst_ptr;
        self->first_q_ptr = &self->pre_queue;
    }
}

/*! \brief   Assigns a receiver callback function for Rx messages
 *  \details This function must be called during initialization time.
 *           The AMS shall not already run.
 *  \param   self          The instance
 *  \param   callback_fptr Reference to the callback function
 *  \param   inst_ptr      Reference to the receiver
 */
void Amd_AssignReceiver(CAmd *self, Amd_RxMsgCompleteCb_t callback_fptr, void *inst_ptr)
{
    if (callback_fptr != NULL)
    {
        self->receive_fptr = callback_fptr;
        self->receive_inst_ptr = inst_ptr;

        if (self->first_receive_fptr == NULL)
        {
            self->first_receive_fptr = callback_fptr;
            self->first_receive_inst_ptr = inst_ptr;
            self->first_q_ptr = &self->rx_queue;
        }
    }
}

/*! \brief   Assigns as callback function which is able to read and modify the Rx message
 *  \param   self          The instance
 *  \param   callback_fptr Reference to the callback function
 *  \param   inst_ptr      Reference to the instance (owner of the callback function)
 */
void Amd_RxAssignModificator(CAmd *self, Amd_RxModificationCb_t callback_fptr, void *inst_ptr)
{
    if (callback_fptr != NULL)
    {
        self->rx_modification_fptr = callback_fptr;
        self->rx_modification_inst_ptr = inst_ptr;
    }
}

/*! \brief   Service function of CAmd
 *  \details The processing of the Rx queues shall be started asynchronously
 *           after the initialization has succeeded.
 *  \param   self          The instance
 */
static void Amd_Service(void *self)
{
    CAmd *self_ = (CAmd*)self;
    Srv_Event_t event_mask;
    Srv_GetEvent(&self_->service, &event_mask);

    if((event_mask & AMD_EV_NOTIFY_RX) == AMD_EV_NOTIFY_RX)     /* triggered on internal transmission */
    {
        Srv_ClearEvent(&self_->service, AMD_EV_NOTIFY_RX);
        if ((self_->started != false) && (self_->first_receive_fptr != NULL))
        {
            uint16_t size = Dl_GetSize(self_->first_q_ptr);
            if (size > 0U)
            {
                self_->first_receive_fptr(self_->first_receive_inst_ptr);
            }
        }
    }

#ifdef AMD_TX_DISTRIB
    if((event_mask & AMD_EV_NOTIFY_TX) == AMD_EV_NOTIFY_TX)     /* notify Tx distribution failure asynchronously */
    {
        Srv_ClearEvent(&self_->service, AMD_EV_NOTIFY_TX);
        Amd_TxProcessNotifyQueue(self_);
    }
#endif
}

/*------------------------------------------------------------------------------------------------*/
/* Events                                                                                         */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Callback function which is invoked on completed application message reception
 *  \param  self        The instance
 *  \param  msg_ptr     Reference to the completed application message
 */
static void Amd_OnAmsComplete(void *self, Ucs_AmsRx_Msg_t *msg_ptr)
{
    CAmd *self_ = (CAmd*)self;

    if (self_->rx_modification_fptr != NULL)
    {
        self_->rx_modification_fptr(self_->rx_modification_inst_ptr, msg_ptr);
    }

    if (self_->first_receive_fptr != NULL)
    {
        Amsg_RxEnqueue(msg_ptr, self_->first_q_ptr);

        if (self_->started != false)
        {
            self_->first_receive_fptr(self_->first_receive_inst_ptr);
        }
    }
    else
    {
        Ams_RxFreeMsg(self_->ams_ptr, msg_ptr);
    }
}

/*! \brief  Callback function if an events leads to the termination of the MNS
 *  \param  self            The instance
 *  \param  error_code_ptr  Reference to the error code
 */
static void Amd_OnTerminateEvent(void *self, void *error_code_ptr)
{
    CAmd *self_ = (CAmd*)self;
    MISC_UNUSED(error_code_ptr);

    TR_INFO((self_->base_ptr->ucs_user_ptr, "[AMD]", "Starting AMD Cleanup", 0U));
    Amd_RxFlush(self_, &self_->pre_queue);
    Amd_RxFlush(self_, &self_->rx_queue);
#ifdef AMD_TX_DISTRIB
    Amd_TxProcessNotifyQueue(self_);
#endif
    TR_INFO((self_->base_ptr->ucs_user_ptr, "[AMD]", "Finished AMD Cleanup", 0U));
}

/*! \brief  Callback function which is invoked if the initialization is complete
 *  \param  self            The instance
 *  \param  error_code_ptr  Reference to the error code
 */
static void Amd_OnEvent(void *self, void *error_code_ptr)
{
    CAmd *self_ = (CAmd*)self;
    MISC_UNUSED(error_code_ptr);

    TR_INFO((self_->base_ptr->ucs_user_ptr, "[AMD]", "Received init complete event", 0U));
    self_->started = true;
    Srv_SetEvent(&self_->service, AMD_EV_NOTIFY_RX);
}

/*! \brief  Flushes a given application Rx message queue
 *  \param  self        The instance
 *  \param  list_ptr    Reference to a list containing application Rx message objects
 */
static void Amd_RxFlush(CAmd *self, CDlList *list_ptr)
{
    Ucs_AmsRx_Msg_t *msg_ptr;

    for (msg_ptr = Amsg_RxDequeue(list_ptr); msg_ptr != NULL; msg_ptr = Amsg_RxDequeue(list_ptr))
    {
        Ams_RxFreeMsg(self->ams_ptr, msg_ptr);
    }
}

/*------------------------------------------------------------------------------------------------*/
/* Pre-processor methods                                                                          */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Peeks the front-most application message from the preprocessing queue
 *  \param  self        The instance
 *  \return Returns a reference to the front-most application message or \c NULL if the queue
 *          is empty.
 */
Ucs_AmsRx_Msg_t* Amd_PrePeekMsg(CAmd *self)
{
    return (Ucs_AmsRx_Msg_t*)(void*)Amsg_RxPeek(&self->pre_queue);
}

/*! \brief  Removes the front-most application message from the preprocessing queue and frees it
 *  \param  self        The instance
 */
void Amd_PreReleaseMsg(CAmd *self)
{
    Ucs_AmsRx_Msg_t *msg_ptr = Amsg_RxDequeue(&self->pre_queue);

    if (msg_ptr != NULL)
    {
        Ams_RxFreeMsg(self->ams_ptr, msg_ptr);
    }
}

/*! \brief  Forwards the front-most application message from the preprocessing queue to the Rx queue
 *  \param  self        The instance
 */
void Amd_PreForwardMsg(CAmd *self)
{
    Ucs_AmsRx_Msg_t *msg_ptr = Amsg_RxDequeue(&self->pre_queue);

    if (msg_ptr != NULL)
    {
        if (self->receive_fptr != NULL)
        {
            Amsg_RxEnqueue(msg_ptr, &self->rx_queue);
            self->receive_fptr(self->receive_inst_ptr);
        }
        else
        {
            Ams_RxFreeMsg(self->ams_ptr, msg_ptr);
        }
    }
}

/*------------------------------------------------------------------------------------------------*/
/* Receiver methods                                                                               */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Peeks the front-most application message from the Rx queue
 *  \param  self        The instance
 *  \return Returns a reference to the front-most application message or \c NULL if the queue
 *          is empty.
 */
Ucs_AmsRx_Msg_t* Amd_RxPeekMsg(CAmd *self)
{
    return (Ucs_AmsRx_Msg_t*)(void*)Amsg_RxPeek(&self->rx_queue);
}

/*! \brief  Removes the front-most application message from the Rx queue and frees it
 *  \param  self        The instance
 */
void Amd_RxReleaseMsg(CAmd *self)
{
    Ucs_AmsRx_Msg_t *msg_ptr = Amsg_RxDequeue(&self->rx_queue);

    if (msg_ptr != NULL)
    {
        Ams_RxFreeMsg(self->ams_ptr, msg_ptr);
    }
}

/*! \brief  Retrieves the number of messages which are appended to the Rx queue
 *  \param  self        The instance
 *  \return Returns the number of messages.
 */
uint16_t Amd_RxGetMsgCnt(CAmd *self)
{
    return Dl_GetSize(&self->rx_queue);
}

#ifdef AMD_TX_DISTRIB
/*------------------------------------------------------------------------------------------------*/
/* Transmitter methods                                                                            */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Distributes a Tx message internally as Rx message
 *  \param  self                    The instance
 *  \param  msg_ptr                 The Tx message
 *  \param  tx_complete_sia_fptr    Single instance API callback function which is invoked as soon as 
 *                                  the transmission was finished.
 *  \param  tx_complete_fptr        Multi instance callback function which is invoked as soon as 
 *                                  the transmission was finished.
 *  \param  tx_complete_inst_ptr    Instance pointer which is referred when tx_complete_fptr is invoked. 
 *  \return Returns \c UCS_RET_SUCCESS if the message accepted for transmission.
 *          Returns \c UCS_RET_ERR_PARAM if the message is refused due to invalid message attributes.
 */
Ucs_Return_t Amd_TxSendMsg(CAmd *self, Ucs_AmsTx_Msg_t *msg_ptr, Amsg_TxCompleteSiaCb_t tx_complete_sia_fptr, 
                           Amsg_TxCompleteCb_t tx_complete_fptr, void* tx_complete_inst_ptr)
{
    Ucs_Return_t ret = UCS_RET_SUCCESS;
    bool tx_internal = false;
    bool tx_network = false;
    bool tx_ignore_nw_failure = false;
    bool tx_check_ni = true;                                    /* if false, transmit to INIC even during NET_OFF  */

    if (Ams_TxIsValidMessage(msg_ptr) != false)
    {
        Net_IsOwnAddrResult_t addr_type = Net_IsOwnAddress(self->net_ptr, msg_ptr->destination_address);

        TR_ASSERT(self->base_ptr->ucs_user_ptr, "[AMS]", (((tx_complete_sia_fptr != NULL) && (tx_complete_fptr != NULL)) == false));
        Amsg_TxSetCompleteCallback(msg_ptr, tx_complete_sia_fptr, tx_complete_fptr, tx_complete_inst_ptr);

        if (msg_ptr->destination_address == UCS_ADDR_INTERNAL)
        {
            tx_internal = true;                                 /* do not forward internal messages to INIC */
        }
        else if ((msg_ptr->fblock_id == AMD_FB_NETBLOCK) || (msg_ptr->fblock_id == AMD_FB_NETWORKMASTER))
        {
            if (addr_type == NET_IS_OWN_ADDR_NODE)              /* replace own node address by "1" to force INIC internal routing */
            {                                                   /* do not replace multicast addresses (these are static and handled by INIC) */
                Amsg_TxReplaceDestinationAddr(msg_ptr, MSG_ADDR_INIC);
            }
            else if ((addr_type == NET_IS_OWN_ADDR_GROUP) ||
                     (msg_ptr->destination_address == UCS_ADDR_BROADCAST_BLOCKING) ||
                     (msg_ptr->destination_address == UCS_ADDR_BROADCAST_UNBLOCKING))
            {
                tx_ignore_nw_failure = true;
            }

            tx_network = true;                                  /* route FBlocks NB and NWM to INIC */
            tx_check_ni = false;                                /* INIC performs checks independent from NI state */
        }
        else if ((msg_ptr->destination_address == UCS_ADDR_BROADCAST_BLOCKING) ||
                (msg_ptr->destination_address == UCS_ADDR_BROADCAST_UNBLOCKING))
        {
            tx_internal = true;                                 /* forward broadcast messages to INIC and distribute internally */
            tx_network = true;
        }
        else
        { 
            switch (addr_type)
            {
                case NET_IS_OWN_ADDR_NODE:
                    tx_internal = true;
                    break;
                case NET_IS_OWN_ADDR_GROUP:
                    tx_internal = true;
                    tx_network = true;
                    break;
                case NET_IS_OWN_ADDR_NONE:
                default:
                    tx_network = true;
                    break;
            }
        }

        if ((Inic_GetAvailability(self->inic_ptr) == UCS_NW_NOT_AVAILABLE) && (tx_check_ni == true))
        { 
            tx_network = false;                                 /* abort network transmission */
            Amsg_TxUpdateResult(msg_ptr, UCS_MSG_STAT_ERROR_NA_OFF);
        }

        if (tx_internal != false)
        {
            if (Amd_TxReceiveInternal(self, msg_ptr) == false)
            {                                                   /* internal transmission failed */
                Amsg_TxUpdateInternalResult(msg_ptr, AMSG_TX_INTRES_ERRBUF);
            }
            else
            {                                                   /* internal transmission succeeded */
                Amsg_TxUpdateInternalResult(msg_ptr, AMSG_TX_INTRES_SUCCESS);
            }
        }
        else if (tx_ignore_nw_failure != false)
        {                                                       /* INIC routing will succeed while NW transmission fails */
            Amsg_TxUpdateInternalResult(msg_ptr, AMSG_TX_INTRES_SUCCESS);
        }

        if (tx_network == false)
        {                                                       /* enqueue message to notification queue and set event */
            Amsg_TxEnqueue(msg_ptr, &self->tx_notify_queue);
            Srv_SetEvent(&self->service, AMD_EV_NOTIFY_TX);
        }
        else
        {
            Ams_TxSendMsgDirect(self->ams_ptr, msg_ptr);
        }
    }
    else
    {
        ret = UCS_RET_ERR_PARAM;                                /* invalid message parameters */
    }

    return ret;
}

/*! \brief  Decides whether to root a message to MCM or RCM FIFO
 *  \param  msg_ptr                 The Tx message object 
 *  \return Returns \c true if a Tx message shall be routed to RCM FIFO, otherwise returns \c false.
 */
static bool Amd_TxIsRcmMsg(Ucs_AmsTx_Msg_t *msg_ptr)
{
    bool ret = false;
    if (((msg_ptr->fblock_id == AMD_FB_NETBLOCK) && (msg_ptr->op_type <= UCS_OP_STARTACK))        /* is NB.Command */
       || ((msg_ptr->fblock_id == AMD_FB_NETWORKMASTER) && (msg_ptr->op_type > UCS_OP_STARTACK))) /* or NWM.Report?*/
    {
        ret = true;
    }

    return ret;
}

/*! \brief  Distributes a Tx message internally as Rx message
 *  \param  self        The instance
 *  \param  msg_ptr     The Tx message
 *  \return Returns \c true if the message distributed successfully.
 *          Returns \c false if the allocation of the Rx message has failed.
 */
static bool Amd_TxReceiveInternal(CAmd *self, Ucs_AmsTx_Msg_t *msg_ptr)
{
    bool ret = false;
    Ucs_AmsRx_Msg_t *rx_ptr = Amsp_AllocRxObj(self->pool_ptr, msg_ptr->data_size);

    if (rx_ptr != NULL)
    {
        uint16_t src_addr = UCS_ADDR_INTERNAL;
        if (msg_ptr->destination_address != UCS_ADDR_INTERNAL)
        {
            src_addr = Inic_GetNodeAddress(self->inic_ptr);
        }
        Amsg_RxBuildFromTx(rx_ptr, msg_ptr, src_addr);
        if (self->first_q_ptr != NULL)
        {
            Amsg_RxEnqueue(rx_ptr, self->first_q_ptr);
            Srv_SetEvent(&self->service, AMD_EV_NOTIFY_RX);
            ret = true;
        }
        else
        {
            Amsp_FreeRxObj(self->pool_ptr, rx_ptr);
            TR_FAILED_ASSERT(self->base_ptr->ucs_user_ptr, "[AMD]");
        }
    }

    return ret;
}

/*! \brief  Notifies the transmission result for all messages in the tx_notify_queue
 *  \param  self        The instance
 */
static void Amd_TxProcessNotifyQueue(CAmd *self)
{
    Ucs_AmsTx_Msg_t *tx_ptr = NULL;

    for (tx_ptr = Amsg_TxDequeue(&self->tx_notify_queue); tx_ptr != NULL; tx_ptr = Amsg_TxDequeue(&self->tx_notify_queue))
    {
        /* just just notify completion, the object is automatically freed to the pool */
        Amsg_TxNotifyComplete(tx_ptr, Amsg_TxGetResultCode(tx_ptr), Amsg_TxGetResultInfo(tx_ptr));
    }
}
#endif

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

