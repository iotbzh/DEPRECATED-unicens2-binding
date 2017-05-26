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
 * \brief Public header file of the Extended Resource Manager.
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_RSM
 * @{
 */

#ifndef UCS_RSM_PB_H
#define UCS_RSM_PB_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_message_pb.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Enumerators                                                                                    */
/*------------------------------------------------------------------------------------------------*/
/*! \brief SyncLost Causes 
 *  \ingroup G_UCS_RSM_TYPES
 */
typedef enum Rsm_SyncLostCause_
{
    RSM_SLC_NWSHUTDOWN,    /*!< \brief "Network Shutdown" causes the SyncLost */
    RSM_SLC_CFGNOTOK,      /*!< \brief "Config Not Ok" causes the SyncLost */
    RSM_SLC_SYSMODIF       /*!< \brief "System Changes like own node address or MPR changes" cause the SyncLost */

} Rsm_SyncLostCause_t;

/*! \brief  RSM Sync states 
 *  \ingroup G_UCS_RSM_TYPES
 */
typedef enum Rsm_DevSyncState_
{
    RSM_DEV_UNSYNCED,  /*!< \brief RSM device is "Unsynced", i.e. not in remote control mode */
    RSM_DEV_SYNCING,   /*!< \brief RSM device is "Synching" */
    RSM_DEV_SYNCED     /*!< \brief RSM device is "Synced", i.e. in remote control mode */

} Rsm_DevSyncState_t;

/*! \brief Result codes of the Extended Resource Manager. 
 *  \ingroup G_UCS_RSM_TYPES
 */
typedef enum Rsm_ResultCode_
{
    RSM_RES_SUCCESS,          /*!< \brief Device Sync succeeded */
    RSM_RES_ERR_SYNC          /*!< \brief Device Sync failed because of not Remote Control Mode */

} Rsm_ResultCode_t;

/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Detailed information used for Extended Resource Manager results. */
typedef struct Rsm_ResultDetails_
{
    /*! \brief Holds the status of the transmission. */
    Ucs_MsgTxStatus_t tx_result;
    /*! \brief Holds the results of the target device. */
    Ucs_StdResult_t inic_result;

} Rsm_ResultDetails_t;

/*! \brief Result structure of the Extended Resource Manager. */
typedef struct Rsm_Result_
{
    /*! \brief Result code. */
    Rsm_ResultCode_t code;
    /*! \brief Detailed information on the returned result. */
    Rsm_ResultDetails_t details;

} Rsm_Result_t;

/*------------------------------------------------------------------------------------------------*/
/* Type definitions                                                                               */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Callback function type to retrieve a RSM result 
 *  \param  self          The instance (optional)
 *  \param  result        The result message object
 *  \ingroup G_UCS_RSM_TYPES
 */
typedef void (*Rsm_ResultCb_t)(void * self, Rsm_Result_t result);

/*!
 * @}
 * \endcond
 */

/*!
 *  \def     UCS_NUM_REMOTE_DEVICES
 *  \brief   Customer assignment for number of remote devices required by Resources Management modules.
 *  \details If the macro is not defined, the UNICENS library will use a default value of 0. The user 
 *           can overwrite this default value by defining the macro. Valid values are in the range 
 *           from 0 to 63. 
  *  \ingroup G_UCS_XRM_CFG
 */ 
#ifndef UCS_NUM_REMOTE_DEVICES
#    define UCS_NUM_REMOTE_DEVICES   0U
#endif

/*! \def UCS_ADDR_LOCAL_DEV
 *  \brief      Defines the address of the local device.
 *  \details    This macro is used to define the address of the local device. It should be used by 
 *              the application to trigger jobs on the local device. 
 *  \ingroup    G_UCS_IRM
 */
#define UCS_ADDR_LOCAL_DEV    0x0001U

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* #ifndef UCS_RSM_PB_H */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

