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
 * \brief Internal header file of Application Message Service
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_AMSC
 * @{
 */

#ifndef UCS_AMS_H
#define UCS_AMS_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_ams_pb.h"
#include "ucs_amsmessage.h"
#include "ucs_amspool.h"
#include "ucs_pool.h"
#include "ucs_transceiver.h"
#include "ucs_pmchannel.h"
#include "ucs_ret_pb.h"
#include "ucs_segmentation.h"
#include "ucs_message.h"
#include "ucs_telqueue.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Internal constants                                                                             */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Defines the default LLR number for Application Messages */
#define AMS_LLRBC_DEFAULT         (MSG_LLRBC_DEFAULT)
/*! \brief Defines the maximum LLR number for Application Messages */
#define AMS_LLRBC_MAX             (MSG_LLRBC_MAX)
/*! \brief Default memory size that is allocated when receiving segmented messages 
 *         without size prefix */
#define AMS_RX_DEF_SIZE_PAYLOAD   400U
/*! \brief Maximum destination address which is reserved for internal transmission */
#define AMS_ADDR_RSVD_RANGE       0x000FU

/*------------------------------------------------------------------------------------------------*/
/* Types                                                                                          */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Callback function type to request FIFO routing for a Tx message.
 *  \param  msg_ptr  Reference to a Tx message object
 *  \return Returns \c true if a Tx message shall be routed to RCM FIFO, otherwise returns \c false.
 */
typedef bool (*Ams_TxIsRcmMsgCb_t)(Ucs_AmsTx_Msg_t *msg_ptr);

/*------------------------------------------------------------------------------------------------*/
/* Class                                                                                          */
/*------------------------------------------------------------------------------------------------*/

/*! \brief      Application Message Service Class
 *  \details    Allows transmission and reception of MOST Application Messages
 */
typedef struct CAms_
{
    CService                service;                    /*!< \brief  Service object */
    CSegmentation           segmentation;               /*!< \brief  Segmentation object */
    CMaskedObserver         unsync_result_observer;     /*!< \brief  Observes un-sync result */

    CBase                  *base_ptr;                   /*!< \brief  Reference to basic services */
    CAmsMsgPool            *pool_ptr;                   /*!< \brief  Pool providing Rx and Tx objects/payload */
    CTransceiver           *trcv_mcm_ptr;               /*!< \brief  Reference to MCM transceiver */
    CTransceiver           *trcv_rcm_ptr;               /*!< \brief  Reference to RCM transceiver */

    struct Ams_tx_
    {
        CDlList             queue;                      /*!< \brief  Input queue of Tx Application Messages */
        uint8_t             default_llrbc;              /*!< \brief  Default LowLevelRetryBlockCount. Valid values: 0..100 */
        Ams_TxIsRcmMsgCb_t  is_rcm_fptr;                /*!< \brief  Assignable callback function to request the correct transceiver */
        uint8_t             next_follower_id;           /*!< \brief  The follower id for the next segmented
                                                         *           message 
                                                         */
    } tx;

    struct Ams_rx_
    {
        CObserver               message_freed_observer; /*!< \brief Observes message freed event */

        Amsg_RxCompleteCb_t     complete_fptr;          /*!< \brief Callback function which is invoked on 
                                                         *          message reception 
                                                         */
        void                   *complete_inst_ptr;      /*!< \brief Instance which is notified on 
                                                         *          message reception 
                                                         */
        CTelQueue               waiting_queue;          /*!< \brief Queue of unprocessed single telegrams */

    } rx;

} CAms;

/*------------------------------------------------------------------------------------------------*/
/* Initialization Methods                                                                         */
/*------------------------------------------------------------------------------------------------*/
extern void Ams_Ctor(CAms *self, CBase *base_ptr, CTransceiver *mcm_trcv_ptr, CTransceiver *rcm_trcv_ptr,
                     CAmsMsgPool *pool_ptr, uint16_t rx_def_payload_sz);

/*------------------------------------------------------------------------------------------------*/
/* Public methods / Tx                                                                            */
/*------------------------------------------------------------------------------------------------*/
extern void Ams_TxSetDefaultRetries(CAms *self, uint8_t llrbc);
extern void Ams_TxAssignMsgFreedObs(CAms *self, CObserver *observer_ptr);
extern void Ams_TxAssignTrcvSelector(CAms *self, Ams_TxIsRcmMsgCb_t cb_fptr);
extern Ucs_AmsTx_Msg_t* Ams_TxGetMsg(CAms *self, uint16_t size);
extern void Ams_TxFreeUnusedMsg(CAms *self, Ucs_AmsTx_Msg_t *msg_ptr);
extern uint16_t Ams_TxGetMsgCnt(CAms *self);
extern bool Ams_TxIsValidMessage(Ucs_AmsTx_Msg_t *msg_ptr);
extern void Ams_TxSendMsgDirect(CAms *self, Ucs_AmsTx_Msg_t *msg_ptr);
extern Ucs_Return_t Ams_TxSendMsg(CAms *self, Ucs_AmsTx_Msg_t *msg_ptr, Amsg_TxCompleteSiaCb_t tx_complete_sia_fptr, 
                                  Amsg_TxCompleteCb_t tx_complete_fptr, void* tx_complete_inst_ptr);

/*------------------------------------------------------------------------------------------------*/
/* Public methods / Rx                                                                            */
/*------------------------------------------------------------------------------------------------*/
extern void Ams_RxAssignReceiver(CAms *self, Amsg_RxCompleteCb_t cb_fptr, void *inst_ptr);
extern void Ams_RxFreeMsg(CAms *self, Ucs_AmsRx_Msg_t *msg_ptr);
extern void Ams_RxOnMcmTelComplete(void *self, Msg_MostTel_t *tel_ptr);
extern void Ams_RxOnRcmTelComplete(void *self, Msg_MostTel_t *tel_ptr);

#ifdef __cplusplus
}               /* extern "C" */
#endif

#endif          /* UCS_AMS_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

