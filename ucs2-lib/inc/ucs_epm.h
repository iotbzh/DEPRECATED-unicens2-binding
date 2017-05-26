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
 * \brief Internal header file of the EndPoint Manager.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_EPM
 * @{
 */


#ifndef UCS_EPM_H
#define UCS_EPM_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_base.h"
#include "ucs_ret_pb.h"
#include "ucs_rm_pb.h"
#include "ucs_xrm.h"
#include "ucs_factory.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*!< \brief  Stores data required by EPM during initialization. */
typedef struct Epm_InitData_
{
    CBase * base_ptr;                               /*!< \brief Reference to a base instance */
    CFactory * fac_ptr;                             /*!< \brief Reference to factory instance */
    Ucs_Rm_XrmResDebugCb_t res_debugging_fptr;      /*!< \brief Reference to the observer callback function for XRM resources */
    Ucs_Xrm_CheckUnmuteCb_t check_unmute_fptr;      /*!< \brief Reference to the callback function pointer to signal "check unmute" of devices */

} Epm_InitData_t;

/*! \brief  Class structure of the EndPoint Management. */
typedef struct CEndpointManagement_
{
    /*!< \brief Reference to a base instance */
    CBase *base_ptr;
    /*!< \brief Reference to factory instance */
    CFactory * fac_ptr;
    /*!< \brief Reference to the application debugging callback function for XRM resources */
    Ucs_Rm_XrmResDebugCb_t res_debugging_fptr;
    /*!< \brief Reference to the callback function pointer to signal "check unmute" of devices */
    Ucs_Xrm_CheckUnmuteCb_t check_unmute_fptr;

} CEndpointManagement;

/*------------------------------------------------------------------------------------------------*/
/* Prototypes of class CEndpointManagement                                                        */
/*------------------------------------------------------------------------------------------------*/
extern void Epm_Ctor(CEndpointManagement * self, Epm_InitData_t * init_ptr);
extern void Epm_InitInternalInfos (CEndpointManagement * self, Ucs_Rm_EndPoint_t * ep_ptr);
extern void Epm_ClearIntInfos(CEndpointManagement * self, Ucs_Rm_EndPoint_t * ep_ptr);
extern void Epm_AddObserver(Ucs_Rm_EndPoint_t * ep_ptr, CObserver * obs_ptr);
extern void Epm_DelObserver(Ucs_Rm_EndPoint_t * ep_ptr, CObserver * obs_ptr);
extern Ucs_Return_t Epm_SetBuildProcess(CEndpointManagement * self, Ucs_Rm_EndPoint_t * ep_ptr);
extern Ucs_Return_t Epm_SetDestroyProcess(CEndpointManagement * self, Ucs_Rm_EndPoint_t * ep_ptr);
extern uint16_t Epm_GetConnectionLabel(CEndpointManagement * self, Ucs_Rm_EndPoint_t * ep_ptr);
extern void Epm_SetConnectionLabel(CEndpointManagement * self, Ucs_Rm_EndPoint_t * ep_ptr, uint16_t conn_label);
extern Ucs_Rm_EndPointState_t Epm_GetState(CEndpointManagement * self, Ucs_Rm_EndPoint_t * ep_ptr);
extern void Epm_ResetState(CEndpointManagement * self, Ucs_Rm_EndPoint_t * ep_ptr);
extern void Epm_ReportShutDown(CEndpointManagement * self);
extern void Epm_ReportInvalidDevice(CEndpointManagement *self, uint16_t destination_address);
extern void Epm_XrmResDebugCb(Ucs_Xrm_ResourceType_t resource_type, Ucs_Xrm_ResObject_t *resource_ptr, 
                              Ucs_Xrm_ResourceInfos_t resource_infos, void *endpoint_inst_ptr, void *user_ptr);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* #ifndef UCS_EPM_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

