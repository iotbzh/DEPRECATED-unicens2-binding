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
 * \brief Internal header file of the API locking module.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_ALM
 * @{
 */

#ifndef UCS_ALM_H
#define UCS_ALM_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_rules.h"
#include "ucs_eh.h"
#include "ucs_timer.h"
#include "ucs_obs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Type definitions                                                                               */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Bitmask used to store locked API methods */
typedef uint32_t Alm_ModuleMask_t;

/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Class structure of the API locking manager */
typedef struct CApiLockingManager_
{
    CTimerManagement *tm_ptr;           /*!< \brief Reference to timer management instance */
    CEventHandler *eh_ptr;              /*!< \brief Reference to event handler instance */
    CTimer garbage_collector;           /*!< \brief Timer for garbage collection */
    CDlList api_list;                   /*!< \brief List of registered APIs */
    void *ucs_user_ptr;         /*!< \brief User reference that needs to be passed in every callback function */
    CMaskedObserver internal_error_obs; /*!< \brief Error observer to handle internal errors and 
                                                    events */

} CApiLockingManager;

/*! \brief  Class structure of the API locking */
typedef struct CApiLocking_
{
    CDlNode node;                       /*!< \brief Node of the doubly linked (API-) list */
    CApiLockingManager *alm_ptr;        /*!< \brief Reference to CApiLockingManager instance */
    Alm_ModuleMask_t method_mask;       /*!< \brief Bitmask which holds locked API methods */
    Alm_ModuleMask_t timeout_mask;      /*!< \brief Bitmask to report timeouts */
    CSingleSubject subject;             /*!< \brief Subject to update registered observer */
    void * ucs_user_ptr;                /*!< \brief UNICENS instance ID */

} CApiLocking;

/*------------------------------------------------------------------------------------------------*/
/* Prototypes of class CApiLockingManager                                                         */
/*------------------------------------------------------------------------------------------------*/
extern void Alm_Ctor(CApiLockingManager *self,
                     CTimerManagement *tm_ptr,
                     CEventHandler *eh_ptr,
                     void * ucs_user_ptr);
extern void Alm_RegisterApi(CApiLockingManager *self, CApiLocking *al_ptr);

/*------------------------------------------------------------------------------------------------*/
/* Prototypes of class CApiLocking                                                                */
/*------------------------------------------------------------------------------------------------*/
extern void Al_Ctor(CApiLocking *self, CSingleObserver *obs_ptr, void * ucs_user_ptr);
extern bool Al_Lock(CApiLocking *self, Alm_ModuleMask_t method);
extern void Al_Release(CApiLocking *self, Alm_ModuleMask_t method);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* #ifndef UCS_ALM_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

