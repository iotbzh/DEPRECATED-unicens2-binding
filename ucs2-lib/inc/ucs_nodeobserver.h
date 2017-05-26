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
 * \brief Internal header file of the CNodeObserver class
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_NODEOBSERVER
 * @{
 */

#ifndef UCS_NODEOBSERVER_H
#define UCS_NODEOBSERVER_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_base.h"
#include "ucs_nodedis.h"
#include "ucs_rtm.h"
#include "ucs_nodeobserver_pb.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Internal constants                                                                             */
/*------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------*/
/* Types                                                                                          */
/*------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------*/
/* Class                                                                                          */
/*------------------------------------------------------------------------------------------------*/

/*! \brief      CNodeObserver Class
 *  \details    Implements the NodeObserver
 */
typedef struct CNodeObserver_
{
    CBase               *base_ptr;              /*!< \brief Reference to base services */
    CNodeDiscovery      *nd_ptr;                /*!< \brief Reference to node discovery */
    CRouteManagement    *rtm_ptr;               /*!< \brief Reference to route management */
    Ucs_Mgr_InitData_t   init_data;             /*!< \brief Initialization data describing nodes and routes*/
    CMaskedObserver      event_observer;        /*!< \brief Observes init complete event */

    Ucs_Signature_t      eval_signature;
    Ucs_Nd_CheckResult_t eval_action;
    Ucs_Rm_Node_t       *eval_node_ptr;
    CTimer               wakeup_timer;          /*!< \brief Timer wakes up processing, sets current 
                                                 *          node available and restarts NodeDiscovery
                                                 */

} CNodeObserver;

/*------------------------------------------------------------------------------------------------*/
/* Methods                                                                                        */
/*------------------------------------------------------------------------------------------------*/
extern void Nobs_Ctor(CNodeObserver *self, CBase *base_ptr, CNodeDiscovery *nd_ptr, CRouteManagement *rtm_ptr, Ucs_Mgr_InitData_t *init_ptr);
extern Ucs_Nd_CheckResult_t Nobs_OnNdEvaluate(void *self, Ucs_Signature_t *signature_ptr);
extern void Nobs_OnNdReport(void *self, Ucs_Nd_ResCode_t code, Ucs_Signature_t *signature_ptr);

#ifdef __cplusplus
}               /* extern "C" */
#endif

#endif          /* UCS_NODEOBSERVER_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

