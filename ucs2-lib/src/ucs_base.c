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
 * \brief Implementation of the Base class.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_BASE
 * @{
 */

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_base.h"
#include "ucs_misc.h"
#include "ucs_message.h"

/*------------------------------------------------------------------------------------------------*/
/* Implementation of class CBase                                                                */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Constructor of the Base class.
 *  \param  self        Instance pointer
 *  \param  init_ptr    Reference to the initialization data
 */
void Base_Ctor(CBase *self, Base_InitData_t *init_ptr)
{
    MISC_MEM_SET(self, 0, sizeof(*self));
    /* Save instance ID and user pointer */
    self->ucs_inst_id = init_ptr->ucs_inst_id;
    self->ucs_user_ptr = init_ptr->ucs_user_ptr;
    /* Create the scheduler instance */
    Scd_Ctor(&self->scd, &init_ptr->scd, init_ptr->ucs_user_ptr);
    /* Create the timer management instance */
    Tm_Ctor(&self->tm, &self->scd, &init_ptr->tm, init_ptr->ucs_user_ptr);
    /* Create the event handler instance */
    Eh_Ctor(&self->eh, init_ptr->ucs_user_ptr);
    /* Create the API locking manager instance */
    Alm_Ctor(&self->alm, &self->tm, &self->eh, init_ptr->ucs_user_ptr);
}

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

