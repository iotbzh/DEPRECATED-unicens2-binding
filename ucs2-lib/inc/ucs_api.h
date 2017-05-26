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

#ifndef UCS_API_H
#define UCS_API_H

/*------------------------------------------------------------------------------------------------*/
/* Version                                                                                        */
/*------------------------------------------------------------------------------------------------*/
/* parasoft suppress item MISRA2004-19_4 reason "No message in public version (PPP issue)" */

/*! \brief UNICENS Major Version Number
 *  \ingroup G_UCS_MISC
 */
#define UCS_VERSION_MAJOR   2

/*! \brief UNICENS Minor Version Number
 *  \ingroup G_UCS_MISC
 */
#define UCS_VERSION_MINOR   1

/*! \brief UNICENS Release Version Number
 *  \ingroup G_UCS_MISC
 */
#define UCS_VERSION_RELEASE 0

/*! \brief UNICENS Build Number
 *  \ingroup G_UCS_MISC
 */
#define UCS_VERSION_BUILD   3491

/* parasoft unsuppress item  MISRA2004-19_4 reason "No message in public version (PPP issue)" */

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_rules.h"
#include "ucs_ret_pb.h"
#include "ucs_lld_pb.h"
#include "ucs_trace_pb.h"

#include "ucs_eh_pb.h"
#include "ucs_class_pb.h"

#endif /* UCS_API_H */
/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

