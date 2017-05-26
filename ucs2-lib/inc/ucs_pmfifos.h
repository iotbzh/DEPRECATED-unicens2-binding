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
 * \brief Internal header file of class CPmFifos
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_PMFIFOS
 * @{
 */

#ifndef UCS_PMFIFOS_H
#define UCS_PMFIFOS_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_rules.h"
#include "ucs_base.h"
#include "ucs_pmfifo.h"
#include "ucs_pmchannel.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Macros                                                                                         */
/*------------------------------------------------------------------------------------------------*/
#define FIFOS_SYNC_TIMEOUT      50U     /*!< \brief Synchronization timeout in milliseconds */
#define FIFOS_SYNC_RETRIES      40U     /*!< \brief Maximum number of synchronization retries after timeout */
#define FIFOS_UNSYNC_TIMEOUT    200U    /*!< \brief Un-synchronization timeout in milliseconds */
#define FIFOS_UNSYNC_RETRIES    0U      /*!< \brief Un-synchronization retries */

/*------------------------------------------------------------------------------------------------*/
/* Internal types                                                                                 */
/*------------------------------------------------------------------------------------------------*/
/*! \brief The synchronization status of all FIFOs */
typedef enum Fifos_SyncState_
{ 
    FIFOS_S_UNSYNCED,               /*!< \brief Not all FIFOs are synchronized  */
    FIFOS_S_SYNCING,                /*!< \brief FIFOs synchronization has started */
    FIFOS_S_SYNCED,                 /*!< \brief All FIFOs are synchronized */
    FIFOS_S_UNSYNCING               /*!< \brief FIFOs un-synchronization has started */

} Fifos_SyncState_t;

/*! \brief  PMS Events */
typedef enum Fifos_Event_
{
    FIFOS_EV_SYNC_LOST = 0,         /*!< \brief Synchronization of at least one FIFO is lost */
    FIFOS_EV_SYNC_ESTABLISHED = 1,  /*!< \brief Synchronization of all FIFOs is established */
    FIFOS_EV_SYNC_FAILED = 2,       /*!< \brief The initial synchronization of FIFOs failed */
    FIFOS_EV_UNSYNC_COMPLETE = 3,   /*!< \brief Un-synchronization of all FIFOs has succeeded */
    FIFOS_EV_UNSYNC_FAILED = 4      /*!< \brief Un-synchronization of all FIFOs has failed */

} Fifos_Event_t;

/*! \brief The class CPmFifos*/
typedef struct CPmFifos_
{
    CBase      *base_ptr;               /*!< \brief Reference to base object */
    uint8_t     ucs_inst_id;            /*!< \brief UNICENS instance ID */
    CPmChannel *channel_ptr;            /*!< \brief UNICENS instance ID */

    CPmFifo *fifos[PMP_MAX_NUM_FIFOS];  /*!< \brief Reference to assigned FIFOs */

    CObserver           obs_icm;        /*!< \brief Observes ICM synchronization state */
    CObserver           obs_rcm;        /*!< \brief Observes ICM synchronization state */
    CObserver           obs_mcm;        /*!< \brief Observes MCM synchronization state */
    Fifos_SyncState_t   state;          /*!< \brief The Overall synchronization state */
    uint8_t             sync_cnt;       /*!< \brief The current count of synchronization command */

    uint8_t             cmd_retries;    /*!< \brief The number of sync/un-sync retries */
    uint16_t            cmd_timeout;    /*!< \brief The the timeout to retry sync/un-sync */
    bool                unsync_initial; /*!< \brief Specifies if un-sync complete shall un-initialize the channel */

    CSubject    event_subject;          /*!< \brief Subject to report synchronization result */
    CTimer      init_timer;             /*!< \brief Timer elapses on synchronization timeout */

} CPmFifos;

/*------------------------------------------------------------------------------------------------*/
/* Function prototypes                                                                            */
/*------------------------------------------------------------------------------------------------*/
extern void Fifos_Ctor(CPmFifos *self, CBase *base_ptr, CPmChannel *channel_ptr, CPmFifo *icm_fifo_ptr, CPmFifo *mcm_fifo_ptr, CPmFifo *rcm_fifo_ptr);
extern void Fifos_AddEventObserver(CPmFifos *self, CObserver *obs_ptr);
extern void Fifos_RemoveEventObserver(CPmFifos *self, CObserver *obs_ptr);
extern void Fifos_Synchronize(CPmFifos *self, bool reset_cnt, bool force_sync);
extern void Fifos_Unsynchronize(CPmFifos *self, bool reset_cnt, bool initial);
extern void Fifos_ForceTermination(CPmFifos *self);
extern void Fifos_ConfigureSyncParams(CPmFifos *self, uint8_t retries, uint16_t timeout);

#ifdef __cplusplus
}                                                   /* extern "C" */
#endif

#endif                                              /* UCS_PMFIFOS_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

