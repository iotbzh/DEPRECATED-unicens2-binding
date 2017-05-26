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
 * \brief Internal header file of Application Message Distributor
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_AMD
 * @{
 */
#ifndef UCS_AMD_H
#define UCS_AMD_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_ams.h"
#include "ucs_base.h"
#include "ucs_amsmessage.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Types                                                                                          */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Assignable function which is invoked as soon as an application message is received 
 *          completely and available in the Rx message queue
 *  \param  self    The instance
 */
typedef void (*Amd_RxMsgCompleteCb_t)(void *self);

/*! \brief  Assignable callback function which is able to read and modify the Rx message
 *  \param  self    The instance
 *  \param  msg_ptr Reference to the Rx message object
 */
typedef void (*Amd_RxModificationCb_t)(void *self, Ucs_AmsRx_Msg_t *msg_ptr);

/*------------------------------------------------------------------------------------------------*/
/* Class                                                                                          */
/*------------------------------------------------------------------------------------------------*/
/*! \brief      Application Message Service Class
 *  \details    Allows transmission and reception of MOST Application Messages
 */
typedef struct CAmd_
{
    CBase                  *base_ptr;                   /*!< \brief  Reference to basic services */
    CAms                   *ams_ptr;                    /*!< \brief  Reference to AMS */
#ifdef AMD_TX_DISTRIB
    CAmsMsgPool            *pool_ptr;                   /*!< \brief  Reference to AMS message pool */
    CInic                  *inic_ptr;                   /*!< \brief  Reference to INIC */
    CNetworkManagement     *net_ptr;                    /*!< \brief  Reference to network management */
#endif
    CService                service;                    /*!< \brief  Service object */
    bool                    started;                    /*!< \brief  Is \c true if the AMD is allowed
                                                         *           to distribute messages
                                                         */
    CMaskedObserver         event_observer;             /*!< \brief  Observes init complete event */
    CMaskedObserver         terminate_observer;         /*!< \brief  Observes events leading to termination */

    void                   *preprocess_inst_ptr;        /*!< \brief Reference to the message preprocessor */
    Amd_RxMsgCompleteCb_t   preprocess_fptr;            /*!< \brief Callback function which is invoked first 
                                                         *          on completed message reception 
                                                         */

    void                   *receive_inst_ptr;           /*!< \brief Reference to the message receiver */
    Amd_RxMsgCompleteCb_t   receive_fptr;               /*!< \brief Callback function which is invoked after 
                                                         *          the preprocessor has finished
                                                         */
#ifdef AMD_TX_DISTRIB
    CDlList                 tx_notify_queue;            /*!< \brief Queue to notify asynchronously a failed transmission */
#endif
    CDlList                 pre_queue;                  /*!< \brief Queue of messages for the preprocessor */
    CDlList                 rx_queue;                   /*!< \brief Queue of messages for the receiver */

    CDlList                *first_q_ptr;                /*!< \brief Reference where to queue the messages first */
    Amd_RxMsgCompleteCb_t   first_receive_fptr;         /*!< \brief Reference of the callback to be fired first */
    void                   *first_receive_inst_ptr;     /*!< \brief Reference to the first receiver */

    Amd_RxModificationCb_t  rx_modification_fptr;       /*!< \brief Callback function for message modification */
    void                   *rx_modification_inst_ptr;   /*!< \brief Callback reference for message modification */

} CAmd;

/*------------------------------------------------------------------------------------------------*/
/* Methods                                                                                        */
/*------------------------------------------------------------------------------------------------*/
extern void Amd_Ctor(CAmd *self, CBase *base_ptr, CAms *ams_ptr);
extern void Amd_AssignPreprocessor(CAmd *self, Amd_RxMsgCompleteCb_t callback_fptr, void *inst_ptr);
extern void Amd_AssignReceiver(CAmd *self, Amd_RxMsgCompleteCb_t callback_fptr, void *inst_ptr);
extern void Amd_RxAssignModificator(CAmd *self, Amd_RxModificationCb_t callback_fptr, void *inst_ptr);

extern Ucs_AmsRx_Msg_t* Amd_PrePeekMsg(CAmd *self);
extern void Amd_PreReleaseMsg(CAmd *self);
extern void Amd_PreForwardMsg(CAmd *self);

extern Ucs_AmsRx_Msg_t* Amd_RxPeekMsg(CAmd *self);
extern void Amd_RxReleaseMsg(CAmd *self);
extern uint16_t Amd_RxGetMsgCnt(CAmd *self);

#ifdef AMD_TX_DISTRIB
Ucs_Return_t Amd_TxSendMsg(CAmd *self, Ucs_AmsTx_Msg_t *msg_ptr, Amsg_TxCompleteSiaCb_t tx_complete_sia_fptr, 
                           Amsg_TxCompleteCb_t tx_complete_fptr, void* tx_complete_inst_ptr);
#endif

#ifdef __cplusplus
}               /* extern "C" */
#endif

#endif          /* UCS_AMD_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

