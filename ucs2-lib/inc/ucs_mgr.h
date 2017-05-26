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
 * \brief Internal header file of the CManager class
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_MGR
 * @{
 */

#ifndef UCS_MGR_H
#define UCS_MGR_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_fsm.h"
#include "ucs_inic.h"
#include "ucs_net.h"
#include "ucs_base.h"
#include "ucs_jobs.h"
#include "ucs_nodedis.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Internal constants                                                                             */
/*------------------------------------------------------------------------------------------------*/
/*! \brief The default value of the desired packet bandwidth for startup command */
#define MGR_PACKET_BW_DEFAULT  52U

/*------------------------------------------------------------------------------------------------*/
/* Types                                                                                          */
/*------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------*/
/* Class                                                                                          */
/*------------------------------------------------------------------------------------------------*/

/*! \brief      Manager Class
 *  \details    Implements the UNICENS Manager State Machine
 */
typedef struct CManager_
{
    bool            listening;                  /*!< \brief Listening is active */
    CFsm            fsm;                        /*!< \brief State machine object */
    CJobService     job_service;
    CSingleObserver job_q_obs;
    CJobQ          *current_q_ptr;
    
    CJobQ           job_q_startup;               
    CJobQ           job_q_force_startup;
 /* CJobQ           job_q_shutdown; */
    CJob            job_startup;
    CJob            job_leave_forced_na;
 /* CJob            job_shutdown; */

    CJob           *list_startup[2];
    CJob           *list_force_startup[3];
 /* CJob           *list_shutdown[2]; */

    CMaskedObserver event_observer;             /*!< \brief Observes init complete event */
    CMaskedObserver nwstatus_mobs;              /*!< \brief Observe network status */

    uint16_t             packet_bw;             /*!< \brief The desired packet bandwidth */
    CBase               *base_ptr;              /*!< \brief Reference to base services */
    CInic               *inic_ptr;              /*!< \brief Reference to class CInic */
    CNetworkManagement  *net_ptr;               /*!< \brief Reference to network management */
    CNodeDiscovery      *nd_ptr;                /*!< \brief Reference to node discovery */

    CSingleObserver startup_obs;                /*!< \brief Startup result callback */
 /* CSingleObserver shutdown_obs; */            /*!< \brief Shutdown result callback */
    CSingleObserver force_na_obs;               /*!< \brief ForceNA result callback */
    bool                 initial;               /*!< \brief Is \c true for the initial network status "available" */

} CManager;

/*------------------------------------------------------------------------------------------------*/
/* Methods                                                                                        */
/*------------------------------------------------------------------------------------------------*/
extern void Mgr_Ctor(CManager *self, CBase *base_ptr, CInic *inic_ptr, CNetworkManagement *net_ptr, CNodeDiscovery *nd_ptr, uint16_t packet_bw);

#ifdef __cplusplus
}               /* extern "C" */
#endif

#endif          /* UCS_MGR_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

