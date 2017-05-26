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
 * \brief Configuration header file of the Extended Resource Manager (XRM).
 */

#ifndef UCS_XRM_CFG_H
#define UCS_XRM_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Definitions                                                                                    */
/*------------------------------------------------------------------------------------------------*/
/* Defines the number of jobs executed by the Extended Resource Manager. Valid range: 1..254. 
 * Default value: 1.
 */
/* #define UCS_XRM_NUM_JOBS             1 */

/* Defines the number of required resources. This value depends on the created resource objects 
 * associated with the jobs that are executed by the Extended Resource Manager. 
 * Valid range: 1..254.  Default value: 8.
 */
/* #define UCS_XRM_NUM_RESOURCES        8 */

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* #ifndef UCS_XRM_CFG_H */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

