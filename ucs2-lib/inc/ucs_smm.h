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
 * \brief Header file of the class CStaticMemoryManager.
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_UCS_SMM_CLASS
 * @{
 */

#ifndef UCS_SMM_H
#define UCS_SMM_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_smm_pb.h"
#include "ucs_ret_pb.h"
#include "ucs_dl.h"
#include "ucs_amsmessage.h"
#include "ucs_amsallocator.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Macros                                                                                         */
/*------------------------------------------------------------------------------------------------*/
#define SMM_NUM_TX_MSGS     ((uint16_t)UCS_AMS_NUM_TX_MSGS)
#define SMM_NUM_RX_MSGS     ((uint16_t)UCS_AMS_NUM_RX_MSGS)
#define SMM_SIZE_TX_MSG     ((uint16_t)UCS_AMS_SIZE_TX_MSG)
#define SMM_SIZE_RX_MSG     ((uint16_t)UCS_AMS_SIZE_RX_MSG)

/*------------------------------------------------------------------------------------------------*/
/* Type definitions                                                                               */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Tx object allocation info */
typedef struct Smm_TxObject_
{
    Amsg_IntMsgTx_t object;             /*!< \brief Tx object memory */
    CDlNode node;                       /*!< \brief Node that enables listing */

} Smm_TxObject_t;

/*! \brief Rx object allocation info */
typedef struct Smm_RxObject_
{
    Amsg_IntMsgRx_t object;             /*!< \brief Rx object memory */
    CDlNode node;                       /*!< \brief Node that enables listing */

} Smm_RxObject_t;

/*! \brief Tx payload allocation info */
typedef struct Smm_TxPayload_
{
    uint8_t data[SMM_SIZE_TX_MSG];      /*!< \brief Tx payload memory */
    CDlNode node;                       /*!< \brief Node that enables listing */

} Smm_TxPayload_t;

/*! \brief Rx payload allocation info */
typedef struct Smm_RxPayload_
{
    uint8_t data[SMM_SIZE_RX_MSG];      /*!< \brief Rx payload memory */
    CDlNode node;                       /*!< \brief Node that enables listing */

} Smm_RxPayload_t;

/*! \brief Static memory allocation of objects and payload */
typedef struct Smm_Resources_
{
    Smm_TxObject_t tx_objects[SMM_NUM_TX_MSGS];  /*!< \brief Statically allocated Tx objects */
    Smm_RxObject_t rx_objects[SMM_NUM_RX_MSGS];  /*!< \brief Statically allocated Rx objects */

    Smm_TxPayload_t tx_payload[SMM_NUM_TX_MSGS]; /*!< \brief Statically allocated Tx payload */
    Smm_RxPayload_t rx_payload[SMM_NUM_RX_MSGS]; /*!< \brief Statically allocated Rx payload */

} Smm_Resources_t;

/*! \brief Descriptor which simplifies access to different memory types */
typedef struct Smm_Descriptor_
{
    CDlList list;           /*!< \brief The list of available memory chunks */
    uint16_t max_mem_size;  /*!< \brief The maximum size of one memory chunk */

} Smm_Descriptor_t;


/*------------------------------------------------------------------------------------------------*/
/* Class definitions                                                                              */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Structure of the class CStaticMemoryManager. */
typedef struct CStaticMemoryManager_
{
    Smm_Resources_t resources;          /*!< \brief Static allocation of memory */

    Smm_Descriptor_t null_descr;        /*!< \brief Descriptor for unknown memory type. Performance measure. */
    Smm_Descriptor_t tx_object_descr;   /*!< \brief Descriptor of Tx object type */
    Smm_Descriptor_t tx_payload_descr;  /*!< \brief Descriptor of Tx payload type */
    Smm_Descriptor_t rx_object_descr;   /*!< \brief Descriptor of Rx object type */
    Smm_Descriptor_t rx_payload_descr;  /*!< \brief Descriptor of Rx payload type */

    void *ucs_user_ptr;                /*!< \brief User reference that needs to be passed in every callback function */

} CStaticMemoryManager;

/*------------------------------------------------------------------------------------------------*/
/* Prototypes of class CStaticMemoryManager                                                     */
/*------------------------------------------------------------------------------------------------*/
extern void Smm_Ctor(CStaticMemoryManager *self, void *ucs_user_ptr);
extern Ucs_Return_t Smm_LoadPlugin(CStaticMemoryManager *self, Ams_MemAllocator_t *allocator_ptr, uint16_t rx_def_payload_size);
extern Ucs_Return_t Smm_GetFreeBufferCnt(CStaticMemoryManager *self, uint16_t *rx_cnt_ptr, uint16_t *tx_cnt_ptr);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* #ifndef UCS_SMM_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

