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
 * \brief Internal header file of Application Message Tx Pool
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_AMTP
 * @{
 */

#ifndef UCS_AMSP_H
#define UCS_AMSP_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_base.h"
#include "ucs_amsmessage.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Types                                                                                          */
/*------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------*/
/* Class                                                                                          */
/*------------------------------------------------------------------------------------------------*/
/*! \brief      Application Message Tx Pool Class
 *  \details    Provides a predefined number of Tx message objects to an internal class
 */
typedef struct CAmtp_
{
    void    *ucs_user_ptr;      /*!< \brief User reference that needs to be passed in every callback function */
    CDlList  msg_queue;         /*!< \brief Queue of messages for the receiver */

} CAmtp;

/*------------------------------------------------------------------------------------------------*/
/* Methods                                                                                        */
/*------------------------------------------------------------------------------------------------*/
extern void Amtp_Ctor(CAmtp *self, Amsg_IntMsgTx_t msg_ptr[], uint8_t data_ptr[], uint8_t msg_cnt, uint16_t payload_sz, void *ucs_user_ptr);
extern Ucs_AmsTx_Msg_t* Amtp_AllocMsg(CAmtp *self);

#ifdef __cplusplus
}               /* extern "C" */
#endif

#endif          /* UCS_AMD_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

