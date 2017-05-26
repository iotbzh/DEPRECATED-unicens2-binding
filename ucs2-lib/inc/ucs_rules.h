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
 * \brief UNICENSV2 API include file
 */

#ifndef UCS_SHARED_CONFIG_H
#define UCS_SHARED_CONFIG_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_cfg.h"

/*------------------------------------------------------------------------------------------------*/
/* Shared Definitions                                                                             */
/*------------------------------------------------------------------------------------------------*/
/*! \def        UCS_FOOTPRINT_TINY
 *  \brief      Define this macro to reduce the RAM and ROM size of the UNICENS software by 
 *              disabling certain features.
 *  \ingroup    G_UCS_INIT_AND_SRV
 *  \details    If this macro is defined the following changes apply:
 *              - Reduction of low-level buffers
 *              - AMS does not support segmentation (payload > 45 bytes)
 *              . 
 */
#ifndef UCS_FOOTPRINT_TINY
# define UCS_FOOTPRINT_TINY
#else
# define AMS_FOOTPRINT_TINY
# define MNSL_FOOTPRINT_TINY
# define SMM_FOOTPRINT_TINY
#endif

#endif /* UCS_SHARED_CONFIG_H */
/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

