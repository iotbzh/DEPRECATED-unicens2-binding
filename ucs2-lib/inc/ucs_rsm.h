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
 * \brief Internal header file of the Remote Sync Manager.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_RSM
 * @{
 */


#ifndef UCS_RSM_H
#define UCS_RSM_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_net.h"
#include "ucs_base.h"
#include "ucs_inic.h"
#include "ucs_ret_pb.h"
#include "ucs_obs.h"
#include "ucs_rsm_pv.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*------------------------------------------------------------------------------------------------*/
/* Enumerations                                                                                   */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  RSM internal state transitions */
typedef enum Rsm_StateTransition_
{
    RSM_ST_IDLE,         /*!< \brief Transition to "Idle" state */
    RSM_ST_SYNC_REQ,     /*!< \brief Transition to "Sync Request" state */
    RSM_ST_NTF_REQ,      /*!< \brief Transition to "Notification Request" state */
    RSM_ST_NTF_CLEAR,    /*!< \brief Transition to "Notification Clear" state */
    RSM_ST_NTF_ALL,      /*!< \brief Transition to "All Notification" state */
    RSM_ST_NTF_GPIO,     /*!< \brief Transition to "Gpio Notification" state */ 
    RSM_ST_SYNC_SUCC,    /*!< \brief Transition to "Sync Success" state */
    RSM_ST_SYNC_ERR      /*!< \brief Transition to "Sync Error" state */

} Rsm_StateTransition_t;

/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Stores data required by RSM during initialization. */
typedef struct Rsm_InitData_
{
    CBase *base_ptr;                /*!< \brief Reference to base instance */
    CInic *inic_ptr;                /*!< \brief Reference to INIC instance */
    CNetworkManagement *net_ptr;    /*!< \brief Reference to Network instance */

} Rsm_InitData_t;

/*! \brief  Stores information required for a RSM device. */
typedef struct Rsm_DeviceInfos_
{
    /*! \brief State of the device */
    Rsm_DevSyncState_t sync_state;
    /*! \brief next state transition */
    Rsm_StateTransition_t next_st;
    /*! \brief stores the current result */
    Rsm_Result_t curr_result;
    /*! \brief stores the current user data that'll be passes to curr_res_cb_fptr */
    void * curr_user_data;
    /*! \brief current result callback function ptr */
    Rsm_ResultCb_t curr_res_cb_fptr;

} Rsm_DeviceInfos_t;

/*! \brief  Stores parameter used for signaling RSM event. */
typedef struct Rsm_EventParam_
{
    /*! \brief own current device address */
    uint16_t own_device_address;
    /*! \brief max node position */
    uint8_t max_node_pos;
    /*! \brief Result observer used for sockets, ports and connections */
    CSingleObserver stdresult_observer;
    /*! \brief Observer used to monitor ICM or MCM Tx Message objects availability */
    CObserver txavailability_observer;
    /*! \brief Observer used to monitor MNS initialization result */
    CMaskedObserver ucsinit_observer;
    /*! \brief Observe MOST Network status in Net module */
    CMaskedObserver nwstatus_observer;
    /*! \brief Own subject to notify the SyncLost event */
    CSubject subject;

} Rsm_EventParam_t;

/*! \brief  Class structure of the Remote Sync Management. */
typedef struct CRemoteSyncManagement_
{
    /*! \brief Reference to an INIC instance */
    CInic *inic_ptr;
    /*! \brief Reference to a base instance */
    CBase *base_ptr;
    /*! \brief Reference to a network instance */
    CNetworkManagement *net_ptr;
    /*! \brief RSM DeviceInfos list */
    Rsm_DeviceInfos_t dev_infos;
    /*! \brief stores the last synclost cause */
    Rsm_SyncLostCause_t last_synclost_cause;
    /*! \brief Parameter object for the RSM Event */
    Rsm_EventParam_t event_param;
    /*! \brief Service instance for the scheduler */
    CService rsm_srv;

} CRemoteSyncManagement;

/*------------------------------------------------------------------------------------------------*/
/* Prototypes of class CRemoteSyncManagement                                                      */
/*------------------------------------------------------------------------------------------------*/
extern void Rsm_Ctor(CRemoteSyncManagement * self, Rsm_InitData_t * init_ptr);
extern void Rsm_AddObserver(CRemoteSyncManagement * self, CObserver * obs);
extern void Rsm_DelObserver(CRemoteSyncManagement * self, CObserver * obs_ptr);
extern Ucs_Return_t Rsm_SyncDev(CRemoteSyncManagement * self, void* user_data, Rsm_ResultCb_t sync_complete_fptr);
extern Rsm_DevSyncState_t Rsm_GetDevState(CRemoteSyncManagement * self);
extern void Rsm_ReportSyncLost (CRemoteSyncManagement * self);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* #ifndef UCS_RSM_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

