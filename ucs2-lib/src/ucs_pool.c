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
 * \brief Implementation of message pool class
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup  G_POOL
 * @{
 */

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_pool.h"
#include "ucs_misc.h"
#include "ucs_trace.h"

/*------------------------------------------------------------------------------------------------*/
/* Internal constants                                                                             */
/*------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------*/
/* Implementation                                                                                 */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Constructor of message pool class
 *  \param  self            The instance
 *  \param  messages        Reference to an array of message objects
 *  \param  size            Number of message objects the \c messages array is comprising.
 *  \param  ucs_user_ptr User reference that needs to be passed in every callback function
 */
void Pool_Ctor(CPool *self, CMessage messages[], uint16_t size, void *ucs_user_ptr)
{
    uint16_t index;

    MISC_MEM_SET(self, 0, sizeof(*self));
    self->ucs_user_ptr = ucs_user_ptr;
    self->initial_size = size;
    self->messages = messages;

    Dl_Ctor(&self->message_list, self->ucs_user_ptr); 

    for (index = 0U; index < size; index++)
    {
        Msg_Ctor(&messages[index]);
        Msg_SetPoolReference(&messages[index], self);
        Dl_InsertTail(&self->message_list, Msg_GetNode(&messages[index]));
    }
}

/*! \brief  Retrieves a message object from the pool
 *  \param  self    The instance
 *  \return Reference to the CMessage structure if a message is available.
 *          Otherwise \c NULL.
 */
CMessage* Pool_GetMsg(CPool *self)
{
    CMessage *msg = NULL;
    CDlNode *node = Dl_PopHead(&self->message_list);

    if (node != NULL)
    {
        msg = (CMessage*)node->data_ptr;
    }

    return msg;
}

/*! \brief  Returns a message object to the pool pre-assigned pool
 *  \param  msg_ptr Reference to the message object which needs
 *                  to be returned to the pool. 
 */
void Pool_ReturnMsg(CMessage *msg_ptr)
{
    CPool *pool_ptr = (CPool*)Msg_GetPoolReference(msg_ptr);

    if (pool_ptr != NULL)
    {
        TR_ASSERT(pool_ptr->ucs_user_ptr, "[POOL]", (Pool_GetCurrentSize(pool_ptr) < pool_ptr->initial_size));
        Dl_InsertTail(&pool_ptr->message_list, Msg_GetNode(msg_ptr));
    }
    else
    {
        TR_ERROR((0U, "[POOL]", "Pool_ReturnMsg(): released msg_ptr=0x%p without pool reference", 1U, msg_ptr));
    }
}

/*! \brief  Retrieves the current number of available message objects in the pool 
 *  \param  self    The instance
 *  \return The current number of available message objects in the pool
 */
uint16_t Pool_GetCurrentSize(CPool *self)
{
    uint16_t list_size = Dl_GetSize(&self->message_list);

    return list_size;
}

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

