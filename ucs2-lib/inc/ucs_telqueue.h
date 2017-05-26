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
 * \brief Declaration of class CTelQueue
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup  G_MSG_QUEUE
 * @{
 */

#ifndef UCS_MSGQUEUE_H
#define UCS_MSGQUEUE_H

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
/* Types                                                                                          */
/*------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------*/
/* Macros                                                                                         */
/*------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------*/
/* Class CTelQueue                                                                                */
/*------------------------------------------------------------------------------------------------*/
/*! \brief   Class CTelQueue 
 *  \details Internal class to queue MOST telegrams
 */
typedef struct CTelQueue_
{
    CDlList     list;           /*! \brief  Doubly linked list */
    void       *ucs_user_ptr;   /*!< \brief User reference that needs to be passed in every callback function */

} CTelQueue;


/*------------------------------------------------------------------------------------------------*/
/* Methods                                                                                        */
/*------------------------------------------------------------------------------------------------*/
extern void Telq_Ctor(CTelQueue *self, void *ucs_user_ptr);
extern Msg_MostTel_t* Telq_Dequeue(CTelQueue *self);
extern Msg_MostTel_t* Telq_Peek(CTelQueue *self);
extern void Telq_Enqueue(CTelQueue *self, Msg_MostTel_t *tel_ptr);
extern uint8_t Telq_GetSize(CTelQueue *self);

#ifdef __cplusplus
}                                                   /* extern "C" */
#endif

#endif /* #ifndef UCS_MSGQUEUE_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

