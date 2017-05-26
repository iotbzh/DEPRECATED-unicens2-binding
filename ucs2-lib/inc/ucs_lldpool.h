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
 * \brief Internal header file of LLD Message Pool
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_PMF
 * @{
 */

#ifndef UCS_LLDPOOL_H
#define UCS_LLDPOOL_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_rules.h"
#include "ucs_base.h"
#include "ucs_lld_pb.h"
#include "ucs_message.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Macros                                                                                         */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Number of LLD Tx handles dedicated to each FIFO */
#define LLDP_NUM_HANDLES              5U

/*------------------------------------------------------------------------------------------------*/
/* Internal types                                                                                 */
/*------------------------------------------------------------------------------------------------*/
/*! \brief   Internal LLD Tx message */
typedef struct Lld_IntTxMsg_
{
    Ucs_Lld_TxMsg_t lld_msg;    /*!< \brief     Contains the public LLD Tx message
                                 *   \details   This attribute needs to be the first one in this structure
                                 */
    CDlNode     node;           /*!< \brief     Node required for queuing */
    CMessage   *msg_ptr;        /*!< \brief     Reference to the associated common message object, or
                                 *              \c NULL if the object is a command */
    void       *owner_ptr;      /*!< \brief     Points to the FIFO which owns the message object 
                                 *              or NULL if the object is a command */

} Lld_IntTxMsg_t;

/*! \brief  Internal LLD Rx message */
typedef struct Lld_IntRxMsg_
{
    Ucs_Lld_RxMsg_t  lld_msg;   /*!< \brief     Contains the public LLD Rx message 
                                 *   \details   This attribute needs to be the first one in this structure
                                 */
    CMessage        *msg_ptr;   /*!< \brief     Reference to the associated common message object*/

} Lld_IntRxMsg_t;

/*! \brief The class CLldPool*/
typedef struct CLldPool_
{ 
    CDlList list;                             /*!< \brief Points to the first available message in Tx pool */
    Lld_IntTxMsg_t messages[LLDP_NUM_HANDLES];/*!< \brief Available messages in Tx pool */

} CLldPool;

/*------------------------------------------------------------------------------------------------*/
/* Function prototypes                                                                            */
/*------------------------------------------------------------------------------------------------*/
extern void Lldp_Ctor(CLldPool *self, void *owner_ptr, void *ucs_user_ptr);
extern void Lldp_ReturnTxToPool(CLldPool *self, Lld_IntTxMsg_t *msg_ptr);
extern Lld_IntTxMsg_t* Lldp_GetTxFromPool(CLldPool *self);


#ifdef __cplusplus
}                                                   /* extern "C" */
#endif

#endif                                              /* UCS_LLDPOOL_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

