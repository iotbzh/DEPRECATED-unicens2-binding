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
 * \brief Implementation of the Command Interpreter.
 *
 * \cond UCS_INTERNAL_DOC
 *
 * \addtogroup  G_UCS_CMD_INT
 * @{
 */


/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_cmd.h"
#include "ucs_misc.h"

/*------------------------------------------------------------------------------------------------*/
/* Internal prototypes                                                                            */
/*------------------------------------------------------------------------------------------------*/


static Ucs_Cmd_Return_t Cmd_SearchMsgId(Ucs_Cmd_MsgId_t msg_id_tab[], uint16_t *index_ptr, 
                                               uint16_t message_id);


/*------------------------------------------------------------------------------------------------*/
/* Implementation                                                                                 */
/*------------------------------------------------------------------------------------------------*/
void Cmd_Ctor(CCmd *self, CBase *base_ptr)
{
    MISC_MEM_SET((void *)self, 0, sizeof(*self));                 /* reset members to "0" */

    self->msg_id_tab_ptr = NULL;
    self->ucs_user_ptr   = base_ptr->ucs_user_ptr;
}


/*! \brief  Add a MessageId Table to the Command Interpreter.
 *  \param  self            Instance pointer
 *  \param  msg_id_tab_ptr    Reference to a MessageId Table
 *  \return  Possible return values are shown in the table below.
 *  Value                           | Description 
 *  ------------------------------- | ------------------------------------
 *  UCS_CMD_RET_SUCCESS             | MessageId Table was successfully added
 *  UCS_CMD_RET_ERR_ALREADY_ENTERED | MessageId Table already added 
 */
Ucs_Cmd_Return_t Cmd_AddMsgIdTable(CCmd *self, Ucs_Cmd_MsgId_t *msg_id_tab_ptr)
{
    Ucs_Cmd_Return_t ret_val = UCS_CMD_RET_SUCCESS;


    if (self->msg_id_tab_ptr != NULL)
    {
        ret_val = UCS_CMD_RET_ERR_ALREADY_ENTERED;
    }
    else
    {
        self->msg_id_tab_ptr = msg_id_tab_ptr;
    }

    return ret_val;
}

/*! \brief   Remove an MessageId Table from the Command Interpreter.
 *  \param   self  Instance pointer of Cmd
 *  \return  Possible return values are shown in the table below.
 *  Value                        | Description 
 *  ---------------------------- | ------------------------------------
 * UCS_CMD_RET_SUCCESS           | MessageId Table was successfully removed
 */
Ucs_Cmd_Return_t Cmd_RemoveMsgIdTable(CCmd *self)
{
    Ucs_Cmd_Return_t ret_val = UCS_CMD_RET_SUCCESS;

    self->msg_id_tab_ptr = NULL;

    return ret_val;
}


/*! \brief  Decode an MCM message 
 *  \param  self            Instance pointer
 *  \param  msg_rx_ptr      Pointer to the message to decode
 *  \return  Possible return values are shown in the table below.
 *  Value                            | Description 
 *  -------------------------------- | ------------------------------------
 *  UCS_CMD_RET_SUCCESS              | decoding was successful
 *  UCS_CMD_RET_ERR_MSGID_NOTAVAIL   | MessageId not found 
 *  UCS_CMD_RET_ERR_TX_BUSY          | no Tx Buffer available
 *  UCS_CMD_RET_ERR_APPL             | error happened in handler function
 *  UCS_CMD_RET_ERR_NULL_PTR         | No MessageId Table available
 */
Ucs_Cmd_Return_t Cmd_DecodeMsg(CCmd *self, Ucs_AmsRx_Msg_t *msg_rx_ptr)
{
    Ucs_Cmd_Return_t result = UCS_CMD_RET_SUCCESS;
    uint16_t         index;

    result = Cmd_SearchMsgId(self->msg_id_tab_ptr, &index, msg_rx_ptr->msg_id);

    if (result == UCS_CMD_RET_SUCCESS)
    {
        /* call handler function */
        result = (Ucs_Cmd_Return_t)(self->msg_id_tab_ptr[index].handler_function_ptr(msg_rx_ptr, self->ucs_user_ptr));
    }

    return result;
}


/*! \brief  Search in a MessageId Table for matching MessageId
 *  \details Function expects that the MessageId Table ends with a termination entry 
 *           (handler_function_ptr == NULL). If this entry is not present, the search may end in an 
 *           endless loop. 
 *  \param   msg_id_tab         MessageId Table
 *  \param   index_ptr          pointer to the matching element
 *  \param   message_id         MessageId
 *  \return  Possible return values are shown in the table below.
 *  Value                           | Description 
 *  ------------------------------- | ------------------------------------
 *  UCS_CMD_RET_SUCCESS             | decoding was successful
 *  UCS_CMD_RET_ERR_MSGID_NOTAVAIL  | MessageId not found
 *  UCS_CMD_RET_ERR_NULL_PTR        | No MessageId Table available
 */
static Ucs_Cmd_Return_t Cmd_SearchMsgId(Ucs_Cmd_MsgId_t msg_id_tab[], uint16_t *index_ptr, 
                                        uint16_t message_id)
{
    Ucs_Cmd_Return_t ret_val = UCS_CMD_RET_SUCCESS;
    uint16_t i = 0U;

    if (msg_id_tab == NULL)
    {
        ret_val = UCS_CMD_RET_ERR_NULL_PTR;
    }
    else
    {
        while (msg_id_tab[i].handler_function_ptr != NULL)        /* last entry */
        {
            if (msg_id_tab[i].msg_id != message_id)
            {
                ++i;                                        /* goto next list element */
            }
            else
            {
                *index_ptr   = i;
                break;
            }
        }

        if (msg_id_tab[i].handler_function_ptr == NULL)               /* no match found */
        {
            ret_val = UCS_CMD_RET_ERR_MSGID_NOTAVAIL;
        }
    }
    return ret_val;
}

/*!
 * @}
 * \endcond
 */





/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

