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
 * \brief Internal header file of the event handler.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_EH
 * @{
 */

#ifndef UCS_EH_H
#define UCS_EH_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_eh_pb.h"
#include "ucs_obs.h"
#include "ucs_trace.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Definitions                                                                                    */
/*------------------------------------------------------------------------------------------------*/
/*! \brief A control FIFO synchronization is lost. When this event occurs the PMS still waits 
 *         until all FIFOs are unsynchronized. So this event is no termination event.
 */
#define EH_E_SYNC_LOST              0x0001U
/*! \brief INIC Build-In-Self-Test failed
 */
#define EH_E_BIST_FAILED            0x0002U
/*! \brief Notifies completed un-synchronization of Port Message FIFOs
 */
#define EH_E_UNSYNC_COMPLETE        0x0004U
/*! \brief Notifies that the Port Message Channel was not able to un-synchronize its FIFOs 
 *         within a definite time
 */
#define EH_E_UNSYNC_FAILED          0x0008U
/*! \brief UNICENS initialization succeeded
 */
#define EH_E_INIT_SUCCEEDED         0x0010U
/*! \brief UNICENS initialization failed
 */
#define EH_E_INIT_FAILED            0x0020U

/*! \brief Mask including all events that lead to the termination of the UCS
 */
#define EH_M_TERMINATION_EVENTS     (EH_E_UNSYNC_COMPLETE | EH_E_UNSYNC_FAILED | \
                                     EH_E_BIST_FAILED | EH_E_INIT_FAILED)

/*! \brief Bitmask to identify all internal event codes
 */
#define EH_M_ALL_EVENTS             (EH_M_TERMINATION_EVENTS | EH_E_INIT_SUCCEEDED | EH_E_SYNC_LOST)

/*------------------------------------------------------------------------------------------------*/
/* Type definitions                                                                               */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Function signature used for callback functions which notifies the event handler 
 *         observers.
 *  \param self             Instance pointer
 *  \param event_code       Reported event code
 */
typedef void (*Ehobs_UpdateCb_t)(void *self, uint32_t event_code);

/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Class structure of the event handler. */
typedef struct CEventHandler_
{
    /*! \brief Subject used for internal events */
    CSubject internal_event_subject;
    /*! \brief Single subject to report error to application */
    CSingleSubject public_error_subject;
    /*! \brief UNICENS instance ID */
    void * ucs_user_ptr;

} CEventHandler;

/*------------------------------------------------------------------------------------------------*/
/* Prototypes of class CEventHandler                                                              */
/*------------------------------------------------------------------------------------------------*/
extern void Eh_Ctor(CEventHandler *self, void *ucs_user_ptr);
extern void Eh_AddObsrvPublicError(CEventHandler *self, CSingleObserver *obs_ptr);
extern void Eh_DelObsrvPublicError(CEventHandler *self);
extern void Eh_ReportEvent(CEventHandler *self, uint32_t event_code);
extern void Eh_AddObsrvInternalEvent(CEventHandler *self, CMaskedObserver *obs_ptr);
extern void Eh_DelObsrvInternalEvent(CEventHandler *self, CMaskedObserver *obs_ptr);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* #ifndef UCS_EH_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

