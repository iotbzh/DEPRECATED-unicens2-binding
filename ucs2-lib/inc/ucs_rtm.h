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
 * \brief Internal header file of the Route Manager.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_RTM
 * @{
 */

#ifndef UCS_RTM_H
#define UCS_RTM_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_base.h"
#include "ucs_ret_pb.h"
#include "ucs_obs.h"
#include "ucs_epm.h"
#include "ucs_net.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Stores data required by RTM during initialization. */
typedef struct Rtm_InitData_
{
    CBase *base_ptr;                /*!< \brief Reference to base instance */
    CEndpointManagement *epm_ptr;   /*!< \brief Reference to the endpoint management instance */
    CNetworkManagement *net_ptr;    /*!< \brief Reference to Network instance */
    Ucs_Rm_ReportCb_t report_fptr;  /*!< \brief Reference to the report callback function */

} Rtm_InitData_t;

/*! \brief  Class structure of the Route Management. */
typedef struct CRouteManagement_
{
    /*! \brief Reference to a base instance */
    CBase *base_ptr;
    /*! \brief Reference to a network instance */
    CEndpointManagement * epm_ptr;
    /*!< \brief Reference to the timer management */ 
    CTimerManagement * tm_ptr;
    /*!< \brief Reference to Network instance */
    CNetworkManagement *net_ptr;
    /*!< \brief Timer for checking routes process */
    CTimer route_check;
    /*!< \brief Reference to the routes list */
    Ucs_Rm_Route_t * routes_list_ptr;
    /*! \brief Points to the current routes to be handled */
    Ucs_Rm_Route_t * curr_route_ptr;
    /*! \brief Current route index */
    uint16_t curr_route_index;
    /*! \brief Size of the current routes list */
    uint16_t routes_list_size;
    /*! \brief Service instance for the scheduler */
    CService rtm_srv;
    /*! \brief Report callback of the routes list */
    Ucs_Rm_ReportCb_t report_fptr;
    /*! \brief Observe MOST Network status in Net module */
    CMaskedObserver nwstatus_observer;
    /*! \brief Observer used to monitor UCS initialization result */
    CMaskedObserver ucsinit_observer;
    /*! \brief Observer used to monitor UCS termination event */
    CMaskedObserver ucstermination_observer;
    /*! \brief Specifies used to monitor UCS termination event */
    bool ucs_is_stopping;
    /*! \brief specifies whether the network status is available or not */
    bool nw_available;
    /*! \brief Flag to lock the API */
    bool lock_api;

} CRouteManagement;

/*------------------------------------------------------------------------------------------------*/
/* Prototypes of class CRouteManagement                                                           */
/*------------------------------------------------------------------------------------------------*/
extern void Rtm_Ctor(CRouteManagement * self, Rtm_InitData_t * init_ptr);
extern Ucs_Return_t Rtm_StartProcess(CRouteManagement * self,  Ucs_Rm_Route_t routes_list[], uint16_t size);
extern Ucs_Return_t Rtm_DeactivateRoute(CRouteManagement * self, Ucs_Rm_Route_t * route_ptr);
extern Ucs_Return_t Rtm_ActivateRoute(CRouteManagement * self, Ucs_Rm_Route_t * route_ptr);
extern Ucs_Return_t Rtm_SetNodeAvailable(CRouteManagement * self, Ucs_Rm_Node_t *node_ptr, bool available);
extern bool Rtm_GetNodeAvailable(CRouteManagement * self, Ucs_Rm_Node_t *node_ptr);
extern Ucs_Return_t Rtm_GetAttachedRoutes(CRouteManagement * self, Ucs_Rm_EndPoint_t * ep_inst, Ucs_Rm_Route_t * ext_routes_list[], uint16_t size_list);
extern uint16_t Rtm_GetConnectionLabel(CRouteManagement * self, Ucs_Rm_Route_t * route_ptr);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* #ifndef UCS_RTM_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

