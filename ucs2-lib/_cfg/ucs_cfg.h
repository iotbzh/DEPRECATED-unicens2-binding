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

#ifndef UCS_CFG_H
#define UCS_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_types_cfg.h"

/*------------------------------------------------------------------------------------------------*/
/* Multi Instance API                                                                             */
/*------------------------------------------------------------------------------------------------*/
/* Number of API instances which can be created by function Ucs_CreateInstance().
 * One API instance is used to communicate with one local INIC. In this case the application
 * is connected to one network.
 * It is possible access multiple networks by having multiple API instances. Each API instance
 * requires communication with an exclusive INIC.
 * Valid range: 1..10. Default value: 1.
 */
/* #define UCS_NUM_INSTANCES                1 */

/*------------------------------------------------------------------------------------------------*/
/* Resources Management                                                                           */
/*------------------------------------------------------------------------------------------------*/
/* Maximum number of remote devices used by Resources Management modules.
 * Valid range: 0..63. Default value: 0.
 */
/* #define UCS_NUM_REMOTE_DEVICES            0U */

/*------------------------------------------------------------------------------------------------*/
/* Application Messages                                                                           */
/*------------------------------------------------------------------------------------------------*/
/* Defines the number of reserved Rx message objects. 
 * Valid values: 5..255. Default value: 20.
 */
/* #define UCS_AMS_NUM_RX_MSGS              20 */

/* Defines the payload size in bytes which is available for every Rx message object.
 * Valid values: 45..65535. Default value: 45.
 */
/* #define UCS_AMS_SIZE_RX_MSG              45 */

/* Defines the number of reserved Tx message objects.
 * Valid values: 5..255. Default value: 20.
 */
/* #define UCS_AMS_NUM_TX_MSGS              20 */

/* Defines the payload size in bytes which is available for every Tx message object.
 * Valid values: 45..65535. Default value: 45.
 */
/* #define UCS_AMS_SIZE_TX_MSG              45 */

/*------------------------------------------------------------------------------------------------*/
/* Memory Optimization                                                                            */
/*------------------------------------------------------------------------------------------------*/
/* Define the following macros to reduces the RAM and ROM size of the UNICENS software by disabling 
 * certain features. If this macro is defined the following changes apply:
 * - Reduction of low-level buffers
 * - AMS does not support segmentation (payload > 45 bytes)
 */
/* #define UCS_FOOTPRINT_TINY */

/*------------------------------------------------------------------------------------------------*/
/* Tracing & Debugging                                                                            */
/*------------------------------------------------------------------------------------------------*/
/* Define the following macros to map info and error trace output to user defined functions. 
 * The purpose of these functions is debugging. It is not recommended to define these functions 
 * in a production system.
 */
/* #define UCS_TR_ERROR     App_TraceError */
/* #define UCS_TR_INFO      App_TraceInfo */

/* extern void App_TraceError(void *ucs_user_ptr, const char module_str[], const char entry_str[], uint16_t vargs_cnt, ...); */
/* extern void App_TraceInfo(void *ucs_user_ptr, const char module_str[], const char entry_str[], uint16_t vargs_cnt, ...); */

#ifdef __cplusplus
}
#endif

#endif /* UCS_CFG_H */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

