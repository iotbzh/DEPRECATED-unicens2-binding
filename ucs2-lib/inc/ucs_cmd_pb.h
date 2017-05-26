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
 * \brief Header file of the Command Interpreter.
 *
 */


#ifndef UCS_CMD_PB_H
#define UCS_CMD_PB_H

#include "ucs_ams_pb.h"

#ifdef __cplusplus
extern "C"
{
#endif



/*------------------------------------------------------------------------------------------------*/
/* Constants                                                                                          */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Denotes the end of an MessageId Table
 *  \ingroup G_UCS_CMD_TYPES
 */
#define UCS_CMD_MSGID_TERMINATION   0xFFFFU



/*------------------------------------------------------------------------------------------------*/
/* Enumerators                                                                                    */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Result codes used for Command Interpreter API functions
 *  \ingroup G_UCS_CMD_TYPES
 */
typedef enum Ucs_Cmd_Return_
{
    UCS_CMD_RET_SUCCESS             = 0x00,     /*!< Operation successfully completed */
    UCS_CMD_RET_ERR_MSGID_NOTAVAIL  = 0x01,     /*!< MessageId not found */
    UCS_CMD_RET_ERR_TX_BUSY         = 0x02,     /*!< No free Tx buffer available */
    UCS_CMD_RET_ERR_APPL            = 0x03,     /*!< Application handler function reports custom error */
    UCS_CMD_RET_ERR_ALREADY_ENTERED = 0x04,     /*!< MessageId Table already connected */
    UCS_CMD_RET_ERR_NULL_PTR        = 0x05      /*!< NULL pointer used as argument */
} Ucs_Cmd_Return_t;

/*------------------------------------------------------------------------------------------------*/
/* Types                                                                                          */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Type definition of user handler functions
 *  \param   msg_rx_ptr     Reference to the received message
 *  \param   user_ptr      User reference provided in \ref Ucs_InitData_t "Ucs_InitData_t::user_ptr"
 *  \return  Possible return values are shown in the table below.
 *  Value                           | Description 
 *  ------------------------------- | ------------------------------------
 *  UCS_CMD_RET_SUCCESS             | The handler function succeeded.
 *  UCS_CMD_RET_ERR_TX_BUSY         | The handler function could not send an answer because no free Tx Buffer was available.
 *  UCS_CMD_RET_ERR_APPL            | An error happened in handler function. 
 *  \note    The application must not return other values than the ones listed above.
 *  \ingroup G_UCS_CMD_TYPES
 */
typedef Ucs_Cmd_Return_t (*Ucs_Cmd_Handler_Function_t)(Ucs_AmsRx_Msg_t *msg_rx_ptr, void *user_ptr);



/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/

/*! \brief   Structure of a single element of the MessageId Table
 *  \details The application provides a MessageId Table which contains all supported MessageIds
 *           with their belonging handler functions. The MessageId Table is an array of several 
 *           Ucs_Cmd_MsgId_t elements. It has to end with a termination entry with the 
 *           value {\ref UCS_CMD_MSGID_TERMINATION, NULL}.
 *  \ingroup G_UCS_CMD_TYPES
 */
typedef struct Ucs_Cmd_MsgId_
{
    /*! \brief MessageId */
    uint16_t msg_id;
    /*! \brief Pointer to the belonging handler function */
    Ucs_Cmd_Handler_Function_t handler_function_ptr;

} Ucs_Cmd_MsgId_t;




/*------------------------------------------------------------------------------------------------*/
/* Prototypes                                                                                     */
/*------------------------------------------------------------------------------------------------*/



#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* UCS_CMD_PB_H */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

