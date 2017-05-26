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
 * \brief Implementation of LLD Message Pool
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup  G_PMF
 * @{
 */

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_lldpool.h"
#include "ucs_misc.h"
#include "ucs_trace.h"

/*------------------------------------------------------------------------------------------------*/
/* Implementation                                                                                 */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Returns an unused LLD Tx message object back to the pool
 *  \param  self        The instance
 *  \param  owner_ptr   Assigns messages to the respective FIFO
 *  \param  ucs_user_ptr User reference that needs to be passed in every callback function
 */
void Lldp_Ctor(CLldPool *self, void *owner_ptr, void* ucs_user_ptr)
{
    uint8_t cnt;
    MISC_MEM_SET(self, 0, sizeof(*self));

    Dl_Ctor(&self->list, ucs_user_ptr);

    for (cnt = 0U; cnt < LLDP_NUM_HANDLES; cnt++)         /* setup LLD Tx handles */
    {
        TR_ASSERT(ucs_user_ptr, "[FIFO]", (self->messages[cnt].msg_ptr == NULL) );
        Dln_Ctor(&self->messages[cnt].node, &self->messages[cnt]);
        self->messages[cnt].owner_ptr = owner_ptr;
        Dl_InsertTail(&self->list, &self->messages[cnt].node);
    }
}

/*! \brief  Returns an unused LLD Tx message object back to the pool
 *  \param  self    The instance
 *  \param  msg_ptr The unused LLD Tx message object 
 */
void Lldp_ReturnTxToPool(CLldPool *self, Lld_IntTxMsg_t *msg_ptr)
{
    Dl_InsertTail(&self->list, &msg_ptr->node);
}

/*! \brief  Allocates an unused LLD Tx message object from the pool
 *  \param  self    The instance
 *  \return An internal LLD Tx message object or \c NULL if no message object is 
 *          available.
 */
Lld_IntTxMsg_t* Lldp_GetTxFromPool(CLldPool *self)
{
    CDlNode *node_ptr = NULL;
    Lld_IntTxMsg_t *handle_ptr = NULL;

    node_ptr = Dl_PopHead(&self->list);

    if (node_ptr != NULL)
    {
        handle_ptr = (Lld_IntTxMsg_t*)Dln_GetData(node_ptr);
    }

    return handle_ptr;
}

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

