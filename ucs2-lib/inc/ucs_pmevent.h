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
 * \brief Internal header file of Port Message Event Handler
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_PMEH
 * @{
 */

#ifndef UCS_PMEV_H
#define UCS_PMEV_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_pmfifos.h"
#include "ucs_base.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Macros                                                                                         */
/*------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------*/
/* Class attributes                                                                               */
/*------------------------------------------------------------------------------------------------*/

/*! \brief   Structure of a Port Message Event Handler */
typedef struct CPmEventHandler_
{ 
    CBase          *base_ptr;                   /*!< \brief Reference to base object */
    CPmFifos       *fifos_ptr;                  /*!< \brief Reference to FIFOs handler */
    CObserver       observer;                   /*!< \brief Observer of PMS events */
    CMaskedObserver sys_observer;               /*!< \brief Observer of system events */

} CPmEventHandler;


/*------------------------------------------------------------------------------------------------*/
/* Class methods                                                                                  */
/*------------------------------------------------------------------------------------------------*/
extern void Pmev_Ctor(CPmEventHandler *self, CBase *base_ptr, CPmFifos *fifos_ptr);
extern void Pmev_Start(CPmEventHandler *self);
extern void Pmev_Stop(CPmEventHandler *self);

#ifdef __cplusplus
}                                                   /* extern "C" */
#endif

#endif                                              /* UCS_PMEV_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

