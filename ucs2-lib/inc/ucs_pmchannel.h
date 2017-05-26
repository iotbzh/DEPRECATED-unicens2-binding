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
 * \brief Internal header file of Port Message Channel
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_PMC
 * @{
 */

#ifndef UCS_PMCHANNEL_H
#define UCS_PMCHANNEL_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_rules.h"
#include "ucs_lld_pb.h"
#include "ucs_lldpool.h"
#include "ucs_pool.h"
#include "ucs_base.h"
#include "ucs_message.h"
#include "ucs_pmp.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Macros                                                                                         */
/*------------------------------------------------------------------------------------------------*/
#define PMCH_POOL_SIZE_RX_MIN       10U  /*!< \brief Minimal size of Rx pool which is shared by all FIFOs */
#define PMCH_POOL_SIZE_RX_OPT       35U  /*!< \brief Optimal size of Rx pool which is shared by all FIFOs */

#define PMCH_MCM_CREDITS_OPT        21U  /*!< \brief Optimal number of credits configured for MCM FIFO */
#define PMCH_MCM_THRESHOLD_OPT      8U   /*!< \brief Optimal threshold configured for MCM FIFO */

#define PMCH_FIFO_CREDITS_OPT       5U   /*!< \brief Optimal number of credits configured for conventional FIFOs */
#define PMCH_FIFO_THRESHOLD_OPT     4U   /*!< \brief Optimal threshold configured for conventional FIFO */

#define PMCH_FIFO_CREDITS_MIN       3U   /*!< \brief Minimal number of credits configured for conventional FIFOs */
#define PMCH_FIFO_THRESHOLD_MIN     2U   /*!< \brief Minimal threshold configured for conventional FIFO */

/* required rules */
#if defined(MNSL_FOOTPRINT_TINY) && defined(MNSL_CHANNEL_POOL_SIZE_RX)
# error Forbidden combination of macros MNSL_FOOTPRINT_TINY and MNSL_CHANNEL_POOL_SIZE_RX
#endif

#ifdef MNSL_CHANNEL_POOL_SIZE_RX
# if (MNSL_CHANNEL_POOL_SIZE_RX < PMCH_POOL_SIZE_RX_MIN)
#   error MNSL_CHANNEL_POOL_SIZE_RX must be at least 10
# endif
#endif

/*! \def     MNSL_CHANNEL_POOL_SIZE_RX
 *  \brief   MNSL configuration that defines the number of pre-allocated Rx messages which are shared by all FIFOs.
 *           Valid values: 35...65535. Default value: 35.
 *
 *  \def     PMCH_POOL_SIZE_RX
 *  \brief   Defines the number of pre-allocated Rx messages which are shared by all FIFOs.
 */
#ifdef MNSL_FOOTPRINT_TINY
# define PMCH_POOL_SIZE_RX          (PMCH_POOL_SIZE_RX_MIN)
# define PMCH_MCM_CREDITS           (PMCH_FIFO_CREDITS_MIN)
# define PMCH_FIFO_CREDITS          (PMCH_FIFO_CREDITS_MIN)
# define PMCH_MCM_THRESHOLD         (PMCH_FIFO_THRESHOLD_MIN)
# define PMCH_FIFO_THRESHOLD        (PMCH_FIFO_THRESHOLD_MIN)
# define MNSL_CHANNEL_POOL_SIZE_RX  (PMCH_POOL_SIZE_RX_MIN)
#elif defined MNSL_CHANNEL_POOL_SIZE_RX
# define PMCH_POOL_SIZE_RX          ((uint16_t)MNSL_CHANNEL_POOL_SIZE_RX)
# define PMCH_MCM_CREDITS           (PMCH_FIFO_CREDITS_MIN)
# define PMCH_FIFO_CREDITS          (PMCH_FIFO_CREDITS_MIN)
# define PMCH_MCM_THRESHOLD         (PMCH_FIFO_THRESHOLD_MIN)
# define PMCH_FIFO_THRESHOLD        (PMCH_FIFO_THRESHOLD_MIN)
#else
# define PMCH_POOL_SIZE_RX          (PMCH_POOL_SIZE_RX_OPT)
# define PMCH_MCM_CREDITS           (PMCH_MCM_CREDITS_OPT)
# define PMCH_FIFO_CREDITS          (PMCH_FIFO_CREDITS_OPT)
# define PMCH_MCM_THRESHOLD         (PMCH_MCM_THRESHOLD_OPT)
# define PMCH_FIFO_THRESHOLD        (PMCH_FIFO_THRESHOLD_OPT)
# define MNSL_CHANNEL_POOL_SIZE_RX  (PMCH_POOL_SIZE_RX_OPT)
#endif

/*------------------------------------------------------------------------------------------------*/
/* Types                                                                                          */
/*------------------------------------------------------------------------------------------------*/
typedef void (*Pmch_OnRxMsg_t)(void *fifo_ptr, CMessage *msg_ptr);
typedef void (*Pmch_OnTxRelease_t)(void *fifo_ptr, Ucs_Lld_TxMsg_t *handle_ptr);

/*! \brief  Initialization structure of the Base Module. */
typedef struct Pmch_InitData_
{
    void *ucs_user_ptr;         /*!< \brief User reference that needs to be passed in every callback function */
    Ucs_Lld_Callbacks_t lld_iface;              /*!< \brief LLD callback functions */
    Pmch_OnTxRelease_t tx_release_fptr;         /*!< \brief Callback which releases a FIFO dedicated LLD buffer */

} Pmch_InitData_t;

/*! \brief  Combination of callback and instance for a receiving FIFO */
typedef struct Pmch_Receiver_
{
    Pmch_OnRxMsg_t rx_fptr;                     /*!< \brief Reference to an Rx callback function */
    void *inst_ptr;                             /*!< \brief Reference to the instance which shall be 
                                                 *          passed to the callback function */
} Pmch_Receiver_t;

/*------------------------------------------------------------------------------------------------*/
/* Class attributes                                                                               */
/*------------------------------------------------------------------------------------------------*/
/*! \brief   Structure of a PMS object */
typedef struct CPmChannel_
{ 
    Pmch_InitData_t init_data;                      /*!< \brief Copy of initialization data */

    Lld_IntRxMsg_t  lld_rx_msgs[PMCH_POOL_SIZE_RX]; /*!< \brief Pre-allocated LLD Rx message objects */
    CMessage        rx_msgs[PMCH_POOL_SIZE_RX];     /*!< \brief Pre-allocated Rx message objects */
    CPool           rx_msgs_pool;                   /*!< \brief Pre-allocated Rx message pool */
    bool            rx_trigger_available;           /*!< \brief Triggers LLD callback function if a buffer
                                                     *          is available again.
                                                     */
    bool            lld_active;                     /*!< \brief Determines whether the LLD is running */
    Ucs_Lld_Api_t   ucs_iface;                      /*!< \brief PMS function pointers */

    Pmch_Receiver_t receivers[PMP_MAX_NUM_FIFOS];   /*!< \brief Registered FIFOs for Rx */

} CPmChannel;


/*------------------------------------------------------------------------------------------------*/
/* Class methods                                                                                  */
/*------------------------------------------------------------------------------------------------*/
/* component creation */
extern void Pmch_Ctor(CPmChannel *self, const Pmch_InitData_t *init_ptr);
extern void Pmch_Initialize(CPmChannel *self);
extern void Pmch_Uninitialize(CPmChannel *self);
extern void Pmch_RegisterReceiver(CPmChannel *self, Pmp_FifoId_t fifo_id, Pmch_OnRxMsg_t rx_fptr, void *inst_ptr);
extern void Pmch_Transmit(CPmChannel *self, Ucs_Lld_TxMsg_t *msg_ptr);
extern void Pmch_ReturnRxToPool(void *self, CMessage *msg_ptr);

#ifdef __cplusplus
}                                               /* extern "C" */
#endif

#endif                                          /* UCS_PMCHANNEL_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

