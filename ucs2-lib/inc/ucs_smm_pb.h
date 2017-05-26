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
 * \brief      Header file of the static memory manager plug-in.
 * \addtogroup G_UCS_AMS
 * @{
 */

#ifndef UCS_SMM_API_H
#define UCS_SMM_API_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_rules.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Default configuration                                                                          */
/*------------------------------------------------------------------------------------------------*/
/*! \def     UCS_AMS_NUM_RX_MSGS
 *  \brief   Defines the number of reserved Rx message objects. 
 *           Valid values: 5..255. Default value: 20.
 */
#ifndef UCS_AMS_NUM_RX_MSGS
#   define UCS_AMS_NUM_RX_MSGS  20
#else
#  if (UCS_AMS_NUM_RX_MSGS < 5) || (UCS_AMS_NUM_RX_MSGS > 255)
#    error "UCS_AMS_NUM_RX_MSGS is not properly defined. Choose a value between: 10 and 255."
#  endif
#endif

/*! \def     UCS_AMS_SIZE_RX_MSG
 *  \brief   Defines the payload size in bytes which is available for every Rx message object.
 *           Valid values: 45..65535. Default value: 45.
 */
#ifndef UCS_AMS_SIZE_RX_MSG
#   define UCS_AMS_SIZE_RX_MSG 45
#else
#  if (UCS_AMS_SIZE_RX_MSG >= 45) && (UCS_AMS_SIZE_RX_MSG <= 65535)
#    if defined(SMM_FOOTPRINT_TINY) && (UCS_AMS_SIZE_RX_MSG != 45)
#      error Do not define UCS_AMS_SIZE_RX_MSG together with SMM_FOOTPRINT_TINY.
#    endif
#  else
#    error UCS_AMS_SIZE_RX_MSG is not properly defined. Choose a value between: 45 and 65535.
#  endif
#endif

/*! \def     UCS_AMS_NUM_TX_MSGS
 *  \brief   Defines the number of reserved Tx message objects.
 *           Valid values: 5..255. Default value: 20.
 */
#ifndef UCS_AMS_NUM_TX_MSGS
#   define UCS_AMS_NUM_TX_MSGS  20
#else
#  if (UCS_AMS_NUM_TX_MSGS < 5) || (UCS_AMS_NUM_TX_MSGS > 255)
#    error "UCS_AMS_NUM_TX_MSGS is not properly defined. Choose a value between: 10 and 255."
#  endif
#endif

/*! \def     UCS_AMS_SIZE_TX_MSG
 *  \brief   Defines the payload size in bytes which is available for every Tx message object.
 *           Valid values: 45..65535. Default value: 45.
 */
#ifndef UCS_AMS_SIZE_TX_MSG
#   define UCS_AMS_SIZE_TX_MSG 45
#else
#  if (UCS_AMS_SIZE_TX_MSG >= 45) && (UCS_AMS_SIZE_TX_MSG <= 65535)
#    if defined(SMM_FOOTPRINT_TINY) && (UCS_AMS_SIZE_TX_MSG != 45)
#      error "Do not define UCS_AMS_SIZE_TX_MSG together with SMM_FOOTPRINT_TINY."
#    endif
#  else
#    error "UCS_AMS_SIZE_TX_MSG is not properly defined. Choose a value between: 45 and 65535."
#  endif
#endif

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* #ifndef UCS_SMM_API_H */

/*!
 * @}
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

