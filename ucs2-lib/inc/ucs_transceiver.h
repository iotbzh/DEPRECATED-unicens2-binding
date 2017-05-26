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
 * \brief Declaration of class CTransceiver
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup  G_TRCV
 * @{
 */

#ifndef UCS_TRANSCEIVER_H
#define UCS_TRANSCEIVER_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_message.h"
#include "ucs_pool.h"
#include "ucs_pmfifo.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Types                                                                                          */
/*------------------------------------------------------------------------------------------------*/ 
/*! \brief   Assignable callback function which is invoked for message reception 
 *  \param   self    The instance
 *  \param   tel_ptr Reference to the message object
 */
typedef void (*Trcv_RxCompleteCb_t)(void *self, Msg_MostTel_t *tel_ptr);

/*! \brief   Assignable callback function which is invoked to filter Rx messages
 *  \details Filtering is a synchronous operation. Hence, it is not possible to keep a message
 *           object for delayed processing. The invoked function has to decide whether a 
 *           message shall be discarded and freed to the Rx pool. Therefore, it has to return 
 *           \c true. By returning \ false, the message will be received in the usual way.
 *  \param   self    The instance
 *  \param   tel_ptr Reference to the message object
 *  \return  Returns \c true to discard the message and free it to the pool (no-pass). Otherwise, returns 
 *           \c false (pass).
 */
typedef bool (*Trcv_RxFilterCb_t)(void *self, Msg_MostTel_t *tel_ptr);

/*------------------------------------------------------------------------------------------------*/
/* Macros                                                                                         */
/*------------------------------------------------------------------------------------------------*/
#define TRCV_SIZE_TX_POOL    10U                    /*!< \brief Number of messages in the message pool */

/*------------------------------------------------------------------------------------------------*/
/* Class CTransceiver                                                                             */
/*------------------------------------------------------------------------------------------------*/
/*! \brief   Class CTransceiver 
 *  \details Provides MOST message objects and communication methods to further classes
 */
typedef struct CTransceiver_
{
    CMessage             tx_msgs[TRCV_SIZE_TX_POOL];/*!< \brief Messages in message pool */
    CPool                tx_msg_pool;               /*!< \brief The message pool */
    uint16_t             tx_def_src;                /*!< \brief Default source address for Tx message object */
    void                *ucs_user_ptr;              /*!< \brief User reference that needs to be passed in every callback function */
    uint8_t              own_id;                    /*!< \brief ID of the transceiver required for tracing */
    CPmFifo             *fifo_ptr;                  /*!< \brief Reference to dedicated port message FIFO */

    Trcv_RxCompleteCb_t  rx_complete_fptr;          /*!< \brief Callback function which is invoked on 
                                                     *          message reception 
                                                     */
    void                *rx_complete_inst;          /*!< \brief Instance which is notified on 
                                                     *          message reception 
                                                     */
    Trcv_RxFilterCb_t    rx_filter_fptr;            /*!< \brief Callback function which is invoked 
                                                     *          to filter Rx messages
                                                     */
    void                *rx_filter_inst;            /*!< \brief Instance which is notified to
                                                     *          filter Rx messages
                                                     */
} CTransceiver;

/*------------------------------------------------------------------------------------------------*/
/* Methods                                                                                        */
/*------------------------------------------------------------------------------------------------*/
/* Constructor */
extern void Trcv_Ctor(CTransceiver *self, CPmFifo *fifo_ptr, uint16_t def_src_addr, void *ucs_user_ptr, uint8_t trace_id);
/* Tx */
extern Msg_MostTel_t* Trcv_TxAllocateMsg(CTransceiver *self, uint8_t size);
extern void Trcv_TxSendMsg(CTransceiver *self, Msg_MostTel_t *tel_ptr);
extern void Trcv_TxSendMsgExt(CTransceiver *self, Msg_MostTel_t *tel_ptr, Msg_TxStatusCb_t callback_fptr, void *inst_ptr);
extern void Trcv_TxSendMsgBypass(CTransceiver *self, Msg_MostTel_t *tel_ptr, Msg_TxStatusCb_t callback_fptr, void *inst_ptr);
extern void Trcv_TxReleaseMsg(Msg_MostTel_t *tel_ptr);
extern void Trcv_TxReuseMsg(Msg_MostTel_t *tel_ptr);
/* Rx */
extern void Trcv_RxAssignReceiver(CTransceiver *self, Trcv_RxCompleteCb_t callback_fptr, void *inst_ptr);
extern void Trcv_RxAssignFilter(CTransceiver *self, Trcv_RxFilterCb_t callback_fptr, void *inst_ptr);
extern void Trcv_RxReleaseMsg(CTransceiver *self, Msg_MostTel_t *tel_ptr);
extern void Trcv_RxOnMsgComplete(void *self, CMessage *tel_ptr);

#ifdef __cplusplus
}                                                   /* extern "C" */
#endif

#endif /* #ifndef UCS_TRANSCEIVER_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

