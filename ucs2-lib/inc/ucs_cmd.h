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
 * \brief Internal header file of the Command Interpreter Add-On.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup  G_UCS_CMD_INT
 * @{
 */

 

#ifndef UCS_CMD_H
#define UCS_CMD_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_cmd_pb.h"
#include "ucs_base.h"


#ifdef __cplusplus
extern "C"
{
#endif




/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/

/*! \brief   Class structure of the Command Interpreter
 */
typedef struct CCmd_
{
    Ucs_Cmd_MsgId_t *msg_id_tab_ptr;    /*!< \brief Pointer to table of MessageIds */ 

    void *ucs_user_ptr;                 /*!< \brief User reference for API callback functions */


} CCmd; 




/*------------------------------------------------------------------------------------------------*/
/* Prototypes                                                                                     */
/*------------------------------------------------------------------------------------------------*/
void Cmd_Ctor(CCmd *self, CBase *base_ptr);
Ucs_Cmd_Return_t Cmd_AddMsgIdTable(CCmd *self, Ucs_Cmd_MsgId_t *msg_id_tab_ptr);
Ucs_Cmd_Return_t Cmd_RemoveMsgIdTable(CCmd *self);
Ucs_Cmd_Return_t Cmd_DecodeMsg(CCmd *self, Ucs_AmsRx_Msg_t *msg_rx_ptr);



#ifdef __cplusplus
}   /* extern "C" */
#endif

/*!
 * @}
 * \endcond
 */

#endif  /* #ifndef UCS_CMD_H */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

