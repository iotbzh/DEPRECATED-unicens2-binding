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
 * \brief Implementation of the event handler.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_EH
 * @{
 */

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_eh.h"
#include "ucs_misc.h"

/*------------------------------------------------------------------------------------------------*/
/* Internal prototypes                                                                            */
/*------------------------------------------------------------------------------------------------*/
static bool Eh_EncodeEvent(uint32_t event_code, Ucs_Error_t *public_error_code_ptr);

/*------------------------------------------------------------------------------------------------*/
/* Implementation of class CEventHandler                                                          */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Constructor of the event handler class.
 *  \param self            Instance pointer
 *  \param ucs_user_ptr     User reference that needs to be passed in every callback function
 */
void Eh_Ctor(CEventHandler *self, void * ucs_user_ptr)
{
    MISC_MEM_SET(self, 0, sizeof(*self));
    /* Save UNICENS instance ID */
    self->ucs_user_ptr = ucs_user_ptr;
    /* Initialize subject for internal events */
    Sub_Ctor(&self->internal_event_subject, self->ucs_user_ptr);
    /* Initialize subject for public error reporting */
    Ssub_Ctor(&self->public_error_subject, self->ucs_user_ptr);
}

/*! \brief Adds an observer which reports public errors
 *  \param self        Instance pointer
 *  \param obs_ptr     Reference to an observer
 */
void Eh_AddObsrvPublicError(CEventHandler *self, CSingleObserver *obs_ptr)
{
    (void)Ssub_AddObserver(&self->public_error_subject, obs_ptr);
}

/*! \brief Removes an observer registered by Eh_AddObsrvPublicError().
 *  \param self        Instance pointer
 */
void Eh_DelObsrvPublicError(CEventHandler *self)
{
    Ssub_RemoveObserver(&self->public_error_subject);
}

/*! \brief Reports an event to the event handler.
 *  \param self        Instance pointer
 *  \param event_code  Event code to report
 */
void Eh_ReportEvent(CEventHandler *self, uint32_t event_code)
{
    Ucs_Error_t public_error_code;
    /* Check if event code exists */
    if((event_code & EH_M_ALL_EVENTS) != 0U)
    {
        /* Encode internal event code */
        bool result = Eh_EncodeEvent(event_code, &public_error_code);
        /* Notify all registered observers */
        Msub_Notify(&self->internal_event_subject, &event_code, event_code);
        /* Report error to application? */
        if(result != false)
        {
            Ssub_Notify(&self->public_error_subject, &public_error_code, false);
        }
    }
}

/*! \brief  Encodes an internal event code. Some internal event codes are mapped to public
 *          error codes.
 *  \param  event_code              Internal event code to report
 *  \param  public_error_code_ptr   Returned public error code
 *  \return true if error must be reported to the application, otherwise false
 */
static bool Eh_EncodeEvent(uint32_t event_code, Ucs_Error_t *public_error_code_ptr)
{
    bool ret_val = true;

    /* Translate internal event code into public error code */
    switch(event_code)
    {
        case EH_E_BIST_FAILED:
            *public_error_code_ptr = UCS_GEN_ERR_INIC;
            break;
        case EH_E_UNSYNC_COMPLETE:
        case EH_E_UNSYNC_FAILED:
            *public_error_code_ptr = UCS_GEN_ERR_COMMUNICATION;
            break;
        default:
            ret_val = false;    /* Do not report this event to application. */
            break;
    }

    return ret_val;
}

/*! \brief Registers an observer on the given event code.
 *  \param self        Instance pointer
 *  \param obs_ptr     Reference to the masked-observer object
 */
void Eh_AddObsrvInternalEvent(CEventHandler *self, CMaskedObserver *obs_ptr)
{
    (void)Sub_AddObserver(&self->internal_event_subject, &obs_ptr->parent);
}

/*! \brief Unregisters the given observer from the given event code.
 *  \param self        Instance pointer
 *  \param obs_ptr     Reference to the masked-observer object
 */
void Eh_DelObsrvInternalEvent(CEventHandler *self, CMaskedObserver *obs_ptr)
{
    (void)Sub_RemoveObserver(&self->internal_event_subject, &obs_ptr->parent);
}

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

