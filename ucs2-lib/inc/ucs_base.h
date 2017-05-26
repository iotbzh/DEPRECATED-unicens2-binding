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
 * \brief Internal header file of the Base class.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_BASE
 * @{
 */

#ifndef UCS_BASE_H
#define UCS_BASE_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_rules.h"
#include "ucs_timer.h"
#include "ucs_scheduler.h"
#include "ucs_trace.h"
#include "ucs_eh.h"
#include "ucs_alm.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Initialization structure of the Base class. */
typedef struct Base_InitData_
{
    void *ucs_user_ptr;             /*!< \brief User reference for API callback functions */
    uint8_t ucs_inst_id;            /*!< \brief UNICENS instance ID */
    Scd_InitData_t scd;             /*!< \brief Initialization data of the scheduler */
    Tm_InitData_t tm;               /*!< \brief Initialization data of the timer management */

} Base_InitData_t;

/*! \brief   Class structure of the Base class. */
typedef struct CBase_
{
    CScheduler scd;                 /*!< \brief Scheduler instance */
    CTimerManagement tm;            /*!< \brief Timer management instance */
    CEventHandler eh;               /*!< \brief Event handler instance */
    CApiLockingManager alm;         /*!< \brief API locking manager instance */
    uint8_t ucs_inst_id;            /*!< \brief UNICENS instance ID */
    void *ucs_user_ptr;             /*!< \brief User reference for API callback functions */

} CBase;

/*------------------------------------------------------------------------------------------------*/
/* Prototypes                                                                                     */
/*------------------------------------------------------------------------------------------------*/
void Base_Ctor(CBase *self, Base_InitData_t *init_ptr);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* #ifndef UCS_BASE_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

