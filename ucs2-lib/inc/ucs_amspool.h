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
 * \brief Internal header file of Application Message Pools
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_AMSPOOL
 * @{
 */

#ifndef UCS_AMSPOOL_H
#define UCS_AMSPOOL_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_ams_pb.h"
#include "ucs_obs.h"
#include "ucs_amsallocator.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Classes                                                                                        */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Class members of AMS Pool */
typedef struct CAmsMsgPool_
{
    Ams_MemAllocator_t *allocator_ptr;              /*!< \brief  Interface to memory allocator */
    Ucs_AmsRx_Msg_t    *rx_rsvd_msg_ptr;            /*!< \brief  Pre-allocated Rx message or NULL if no 
                                                     *           reserved message is available */
    Ucs_AmsRx_Msg_t    *rx_rsvd_msg_ref;            /*!< \brief  Stores the reference of the reserved message
                                                     *           to identify it and restore the 
                                                     *           \c rx_rsvd_msg_ptr. */
    CSubject            tx_freed_subject;           /*!< \brief  Allows to observe freed Tx message event */
    CSubject            rx_freed_subject;           /*!< \brief  Allows to observe freed Rx message event */
    bool                tx_notify_freed;            /*!< \brief  Is \c true when to notify the next Tx freed object */
    bool                rx_notify_freed;            /*!< \brief  Is \c true when to notify the next Rx freed object */
    bool                terminated;                 /*!< \brief  Is \c true if a cleanup was done. Helps to release the 
                                                     *           pre-allocated message after the first cleanup attempt. */
    void               *ucs_user_ptr;               /*!< \brief User reference that needs to be passed in every callback function */

} CAmsMsgPool;

/*------------------------------------------------------------------------------------------------*/
/* Class methods                                                                                  */
/*------------------------------------------------------------------------------------------------*/
extern void Amsp_Ctor(CAmsMsgPool *self, Ams_MemAllocator_t *mem_allocator_ptr, void *ucs_user_ptr);
extern void Amsp_Cleanup(CAmsMsgPool *self);
/* Tx */
extern void Amsp_AssignTxFreedObs(CAmsMsgPool *self, CObserver *observer_ptr);
extern Ucs_AmsTx_Msg_t* Amsp_AllocTxObj(CAmsMsgPool *self, uint16_t payload_sz);
/* Rx */
extern void Amsp_AssignRxFreedObs(CAmsMsgPool *self, CObserver *observer_ptr);
extern Ucs_AmsRx_Msg_t* Amsp_AllocRxObj(CAmsMsgPool *self, uint16_t payload_sz);
extern Ucs_AmsRx_Msg_t* Amsp_AllocRxRsvd(CAmsMsgPool *self);
extern bool Amsp_AllocRxPayload(CAmsMsgPool *self, uint16_t payload_sz, Ucs_AmsRx_Msg_t* msg_ptr);
extern void Amsp_FreeRxObj(CAmsMsgPool *self, Ucs_AmsRx_Msg_t* msg_ptr);
extern void Amsp_FreeRxPayload(CAmsMsgPool *self, Ucs_AmsRx_Msg_t* msg_ptr);

#ifdef __cplusplus
}               /* extern "C" */
#endif

#endif          /* UCS_AMSPOOL_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

