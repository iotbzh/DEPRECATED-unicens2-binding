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
 * \brief Declaration of message pool class
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup  G_POOL
 * @{
 */
#ifndef UCS_POOL_H
#define UCS_POOL_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_message.h"
#include "ucs_dl.h"

#ifdef __cplusplus
extern "C"
{
#endif
/*------------------------------------------------------------------------------------------------*/
/* Class CPool                                                                                    */
/*------------------------------------------------------------------------------------------------*/
/*! \brief   Class CMessage 
 *  \details Common internal message class which embeds a list of MOST telegrams 
 */
typedef struct CPool_
{
    uint16_t    initial_size;   /*! \brief  The size of a provided message array */
    CMessage   *messages;       /*! \brief  Reference to a message array provided by another module */
    CDlList     message_list;   /*! \brief  Doubly linked list required providing available messages */
    void *ucs_user_ptr;         /*!< \brief User reference that needs to be passed in every callback function */

} CPool;

/*------------------------------------------------------------------------------------------------*/
/* Methods                                                                                        */
/*------------------------------------------------------------------------------------------------*/
extern void Pool_Ctor(CPool *self, CMessage messages[], uint16_t size, void *ucs_user_ptr);
extern CMessage* Pool_GetMsg(CPool *self);
extern void Pool_ReturnMsg(CMessage *msg_ptr);
extern uint16_t Pool_GetCurrentSize(CPool *self);

#ifdef __cplusplus
}                                                   /* extern "C" */
#endif

#endif /* #ifndef UCS_POOL_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

