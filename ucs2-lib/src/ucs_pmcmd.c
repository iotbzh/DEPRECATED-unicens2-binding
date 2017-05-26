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
 * \brief Implementation of class CPmCommand
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup  G_PM_CMD
 * @{
 */

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_pmcmd.h"
#include "ucs_misc.h"

/*------------------------------------------------------------------------------------------------*/
/* Implementation                                                                                 */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Constructor of CPmCommand class 
 *  \param  self    The instance
 *  \param  fifo    The dedicated FIFO
 *  \param  type    The port message type
 */
void Pmcmd_Ctor(CPmCommand *self, Pmp_FifoId_t fifo, Pmp_MsgType_t type)
{
    MISC_MEM_SET(self, 0, sizeof(*self));           /* setup attributes */
    self->memory.data_ptr = &self->data[0];
    self->tx_obj.lld_msg.memory_ptr = &self->memory;
    self->tx_obj.msg_ptr = NULL;                    /* label message as command by setting */
    self->tx_obj.owner_ptr = NULL;                  /* msg_ptr and owner_ptr to NULL */
    self->trigger = false;

    Pmp_SetPmhl(self->data, 3U);                    /* PMHL is always "3" for control/status messages */
    Pmp_SetFph(self->data, fifo, type);
}

/*! \brief  Retrieves reference to the LLD Tx message object required to call Pmch_Transmit()
 *  \param  self    The instance
 *  \return Returns a reference to the LLD Tx message object 
 */
Ucs_Lld_TxMsg_t* Pmcmd_GetLldTxObject(CPmCommand *self)
{
    return (Ucs_Lld_TxMsg_t*)(void*)self;
}

/*! \brief  Sets the content of a command/status message
 *  \param  self          The instance
 *  \param  sid           The sequence id
 *  \param  ext_type      The ExtType type
 *  \param  ext_code      The ExtType code
 *  \param  add_data_ptr  Additional payload data
 *  \param  add_data_sz   The size of additional payload data, valid values: 0..4
 */
void Pmcmd_SetContent(CPmCommand *self, uint8_t sid, uint8_t ext_type, uint8_t ext_code, uint8_t add_data_ptr[], uint8_t add_data_sz)
{
    if ((add_data_ptr != NULL) && (add_data_sz != 0U))
    {
        MISC_MEM_CPY(&self->data[6U], add_data_ptr, (size_t)add_data_sz);
    }

    self->memory.data_size = 6U + (uint16_t)add_data_sz;
    self->memory.total_size = 6U + (uint16_t)add_data_sz;

    Pmp_SetPml(self->data, 4U + add_data_sz);
    Pmp_SetSid(self->data, sid);
    Pmp_SetExtType(self->data, ext_type, ext_code);
}

/*! \brief   Updates the content of a command/status message
 *  \details The length and the content of the payload is not modified.
 *           It is important to call Pmcmd_SetContent() before.
 *  \param   self        The instance
 *  \param   sid         The sequence id
 *  \param   ext_type    The ExtType type
 *  \param   ext_code    The ExtType code
 */
void Pmcmd_UpdateContent(CPmCommand *self, uint8_t sid, uint8_t ext_type, uint8_t ext_code)
{
    Pmp_SetSid(self->data, sid);
    Pmp_SetExtType(self->data, ext_type, ext_code);
}

/*! \brief  Reserves the command object if it is available
 *  \param  self    The instance
 *  \return \c true if the command object is available, \c false
 *          if the command object is (still) in usage
 */
bool Pmcmd_Reserve(CPmCommand *self)
{
    bool succ = false;

    if (self->reserved == false)
    {
        self->reserved = true;
        succ = true;
    }
    return succ;
}

/*! \brief  Releases the command object after usage
 *  \param  self    The instance
 */
void Pmcmd_Release(CPmCommand *self)
{
    self->reserved = false;
}

/*! \brief  Sets or resets the trigger attribute
 *  \param  self    The instance
 *  \param  trigger The trigger value
 */
void Pmcmd_SetTrigger(CPmCommand *self, bool trigger)
{
    self->trigger = trigger;
}

/*! \brief  Returns the trigger value
 *  \param  self    The instance
 *  \return Returns \c true if the trigger attribute is set, otherwise \c false.
 */
bool Pmcmd_IsTriggered(CPmCommand *self)
{
    return self->trigger;
}

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

