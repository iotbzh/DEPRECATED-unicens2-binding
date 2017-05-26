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
 * \brief Implementation of class CTelQueue
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup  G_MSG_QUEUE
 * @{
 */

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_telqueue.h"
#include "ucs_misc.h"

/*------------------------------------------------------------------------------------------------*/
/* Internal constants                                                                             */
/*------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------*/
/* Implementation                                                                                 */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Constructor of class CTelQueue
 *  \param  self            The instance
 *  \param  ucs_user_ptr User reference that needs to be passed in every callback function
 */
void Telq_Ctor(CTelQueue *self, void *ucs_user_ptr)
{
    self->ucs_user_ptr = ucs_user_ptr;
    Dl_Ctor(&self->list, self->ucs_user_ptr);
}

/*! \brief  Retrieves the head object of the telegram queue
 *  \param  self    The instance
 *  \return Reference to the telegram if a telegram object is available.
 *          Otherwise \c NULL.
 */
Msg_MostTel_t* Telq_Dequeue(CTelQueue *self)
{
    Msg_MostTel_t *tel_ptr = NULL;
    CDlNode *node_ptr = Dl_PopHead(&self->list);

    if (node_ptr != NULL)
    {
        tel_ptr = (Msg_MostTel_t*)Dln_GetData(node_ptr);
    }

    return tel_ptr;
}

/*! \brief  Retrieves a reference to the head object 
 *          without removing it from the telegram queue
 *  \param  self    The instance
 *  \return Reference to the telegram if a telegram object is available.
 *          Otherwise \c NULL.
 */
Msg_MostTel_t* Telq_Peek(CTelQueue *self)
{
    Msg_MostTel_t *tel_ptr = NULL;
    CDlNode *node_ptr = Dl_PeekHead(&self->list);

    if (node_ptr != NULL)
    {
        tel_ptr = (Msg_MostTel_t*)Dln_GetData(node_ptr);
    }

    return tel_ptr;
}

/*! \brief  Adds a telegram to the tail of the queue
 *  \param  self    The instance
 *  \param  tel_ptr Reference to the telegram 
 */
void Telq_Enqueue(CTelQueue *self, Msg_MostTel_t *tel_ptr)
{
    Dl_InsertTail(&self->list, Msg_GetNode((CMessage*)(void*)tel_ptr));
}

/*! \brief  Retrieves the current number of objects in the telegram queue 
 *  \param  self    The instance
 *  \return The current number of available telegram objects in the pool
 */
uint8_t Telq_GetSize(CTelQueue *self)
{
    return (uint8_t)Dl_GetSize(&self->list);
}

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

