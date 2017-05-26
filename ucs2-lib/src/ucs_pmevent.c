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
 * \brief Implementation of Port Message Event Handler
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup  G_PMEH
 * @{
 */

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_pmevent.h"
#include "ucs_misc.h"

/*------------------------------------------------------------------------------------------------*/
/* Internal prototypes                                                                            */
/*------------------------------------------------------------------------------------------------*/
static void Pmev_OnFifosEvent(void *self, void *data_ptr);
static void Pmev_OnSystemEvent(void *self, void *data_ptr);

/*------------------------------------------------------------------------------------------------*/
/* Implementation                                                                                 */
/*------------------------------------------------------------------------------------------------*/

/*! \brief  Constructor of class CPmEventHandler
 *  \param  self        The instance
 *  \param  base_ptr    Reference to base object
 *  \param  fifos_ptr   Reference to CPmFifos object
 */
void Pmev_Ctor(CPmEventHandler *self, CBase *base_ptr, CPmFifos *fifos_ptr)
{
    self->base_ptr = base_ptr;
    self->fifos_ptr = fifos_ptr;

    Obs_Ctor(&self->observer, self, &Pmev_OnFifosEvent);

    Mobs_Ctor(&self->sys_observer, self, (EH_E_BIST_FAILED | EH_E_INIT_FAILED), &Pmev_OnSystemEvent);
    Eh_AddObsrvInternalEvent(&self->base_ptr->eh, &self->sys_observer);
}

/*! \brief  Start reporting events to EH
 *  \param  self    The instance
 */
void Pmev_Start(CPmEventHandler *self)
{
    Fifos_AddEventObserver(self->fifos_ptr, &self->observer);
}

/*! \brief  Stops reporting events to EH
 *  \param  self    The instance
 */
void Pmev_Stop(CPmEventHandler *self)
{
    Fifos_RemoveEventObserver(self->fifos_ptr, &self->observer);
}

/*! \brief  Callback function to handle a PMS event
 *  \param  self        The instance
*   \param  data_ptr    Reference to the PMS event
 */
static void Pmev_OnFifosEvent(void *self, void *data_ptr)
{
    CPmEventHandler *self_ = (CPmEventHandler*)self;
    Fifos_Event_t *event_ptr = (Fifos_Event_t*)data_ptr;

    switch (*event_ptr)
    {
        case FIFOS_EV_SYNC_LOST:
            Eh_ReportEvent(&self_->base_ptr->eh, EH_E_SYNC_LOST);
            break;
        case FIFOS_EV_SYNC_ESTABLISHED:
            /* not relevant */
            break;
        case FIFOS_EV_SYNC_FAILED:
            /* not relevant */
            break;
        case FIFOS_EV_UNSYNC_COMPLETE:
            Eh_ReportEvent(&self_->base_ptr->eh, EH_E_UNSYNC_COMPLETE);
            break;
        case FIFOS_EV_UNSYNC_FAILED:
            Eh_ReportEvent(&self_->base_ptr->eh, EH_E_UNSYNC_FAILED);
            break;
        default:
            /* not relevant */
            break;
    }
}

/*! \brief  Callback function to handle an UCS system events
 *  \param  self        The instance
*   \param  data_ptr    Reference to the system event event
 */
static void Pmev_OnSystemEvent(void *self, void *data_ptr)
{
    CPmEventHandler *self_ = (CPmEventHandler*)self;
    Fifos_ForceTermination(self_->fifos_ptr);
    MISC_UNUSED(data_ptr);
}

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

