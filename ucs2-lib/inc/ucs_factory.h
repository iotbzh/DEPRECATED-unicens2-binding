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
 * \brief Internal header file of the MNS Factory.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_FAC
 * @{
 */


#ifndef UCS_FAC_H
#define UCS_FAC_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_net.h"
#include "ucs_base.h"
#include "ucs_inic.h"
#include "ucs_ret_pb.h"
#include "ucs_rsm.h"
#include "ucs_xrm.h"
#include "ucs_i2c.h"
#include "ucs_gpio.h"
#include "ucs_nsm.h"
#include "ucs_xrmpool.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Definitions                                                                                    */
/*------------------------------------------------------------------------------------------------*/
/*! \def FAC_NUM_DEVICES
 *  \brief      Defines the number of remote devices.
 *  \details    The number of remote devices required by the application for remote jobs. The 
 *              default value is 0 in MNS configuration file ucs_cfg.h. The user can adjust this 
 *              value by defining the macro \ref UCS_NUM_REMOTE_DEVICES. Valid values are in 
 *              the range from 0 to 63.
 *  \ingroup    G_FAC
 */
#define FAC_NUM_DEVICES              ((uint8_t)UCS_NUM_REMOTE_DEVICES + 1U)


/*------------------------------------------------------------------------------------------------*/
/* Enumerations                                                                                   */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Factory Instances type. */
typedef enum Fac_Inst_
{
    FAC_INST_INIC, /*!< \brief   Corresponds to the INIC Instance type */
    FAC_INST_RSM,  /*!< \brief   Corresponds to the RSM Instance type */
    FAC_INST_XRM,  /*!< \brief   Corresponds to the XRM Instance type */
    FAC_INST_GPIO, /*!< \brief   Corresponds to the GPIO Instance type */
    FAC_INST_I2C,  /*!< \brief   Corresponds to the I2C Instance type */
    FAC_INST_NSM   /*!< \brief   Corresponds to the NSM Instance type */
} Fac_Inst_t;

/*------------------------------------------------------------------------------------------------*/
/* Type definitions                                                                               */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Callback signature used by the foreach-function of the factory class.
 *  \param inst_type  The instance type to be looked for.
 *  \param inst_ptr   Reference to the current instance.
 *  \param ud_ptr     Reference to the user data
 *  \return true: Stop the for-each-loop
 *  \return false: Continue the for-each-loop
 */
typedef bool (*Fac_ForeachFunc_t)(Fac_Inst_t inst_type, void *inst_ptr, void *ud_ptr);

/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Stores data required by the Factory during initialization. */
typedef struct Fac_InitData_
{
    CBase *base_ptr;                /*!< \brief Reference to base instance */
    CNetworkManagement *net_ptr;    /*!< \brief Reference to Network instance */
    CXrmPool * xrmp_ptr;            /*!< \brief Reference to the XRM Pool instance */
    CTransceiver * icm_transceiver; /*!< \brief Reference to ICM transceiver */
    CTransceiver * rcm_transceiver; /*!< \brief Reference to MCM transceiver */

} Fac_InitData_t;

/*! \brief  Class structure of the MNS Factory. */
typedef struct CFactory_
{
    /*! \brief FBlock INIC list */
    CInic fbi_list[FAC_NUM_DEVICES];
    /*! \brief XRM list */
    CExtendedResourceManager xrm_list[FAC_NUM_DEVICES];
    /*! \brief RSM list */
    CRemoteSyncManagement rsm_list[FAC_NUM_DEVICES];
    /*! \brief GPIO list */
    CGpio gpio_list[FAC_NUM_DEVICES];
    /*! \brief I2C list */
    CI2c i2c_list[FAC_NUM_DEVICES];
    /*! \brief Node Scripting list */
    CNodeScriptManagement nsm_list[FAC_NUM_DEVICES];
    /*! \brief Reference to a base instance */
    CBase *base_ptr;
    /*! \brief Reference to a network instance */
    CNetworkManagement *net_ptr;
    /*!< \brief Reference to the XRM Pool instance */
    CXrmPool * xrmp_ptr;
    /*! \brief The ICM transceiver */
    CTransceiver * icm_transceiver;
    /*! \brief The RCM transceiver */
    CTransceiver * rcm_transceiver;

} CFactory;

/*------------------------------------------------------------------------------------------------*/
/* Prototypes of class CFactory                                                                   */
/*------------------------------------------------------------------------------------------------*/
extern void Fac_Ctor(CFactory * self, Fac_InitData_t * init_ptr);
extern CExtendedResourceManager * Fac_GetXrm(CFactory * self, uint16_t address, Ucs_Xrm_ResourceDebugCb_t res_debugging_fptr, Ucs_Xrm_CheckUnmuteCb_t check_unmute_fptr);
extern CExtendedResourceManager * Fac_GetXrmByJobList(CFactory * self, UCS_XRM_CONST Ucs_Xrm_ResObject_t *resource_object_list[]);
extern CExtendedResourceManager * Fac_GetXrmLegacy(CFactory * self, uint16_t address, Ucs_Xrm_CheckUnmuteCb_t check_unmute_fptr);
extern CInic * Fac_GetInic(CFactory * self, uint16_t address);
extern CInic * Fac_FindInic(CFactory * self, uint16_t address);
extern CNodeScriptManagement * Fac_GetNsm(CFactory * self, uint16_t address);
extern CNodeScriptManagement * Fac_FindNsm(CFactory * self, uint16_t address);
extern CRemoteSyncManagement * Fac_GetRsm(CFactory * self, uint16_t address);
extern CRemoteSyncManagement * Fac_FindRsm(CFactory * self, uint16_t address);
extern CGpio * Fac_GetGpio(CFactory * self, uint16_t address, Ucs_Gpio_TriggerEventResultCb_t trigger_event_status_fptr);
extern CI2c * Fac_GetI2c(CFactory * self, uint16_t address, Ucs_I2c_IntEventReportCb_t i2c_interrupt_report_fptr);
extern void Fac_Foreach(CFactory * self, Fac_Inst_t inst_type, Fac_ForeachFunc_t func_ptr, void *user_data_ptr);

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

