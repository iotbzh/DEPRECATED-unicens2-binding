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
 * \brief Internal header file of class CSysDiag.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_SYS_DIAG
 * @{
 */

#ifndef UCS_SYS_DIAG_H
#define UCS_SYS_DIAG_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_obs.h"
#include "ucs_fsm.h"
#include "ucs_inic.h"
#include "ucs_exc.h"


#ifdef __cplusplus
extern "C"
{
#endif




/*------------------------------------------------------------------------------------------------*/
/* Enumerations                                                                                   */
/*------------------------------------------------------------------------------------------------*/
/*! \brief   Result codes of a tested segment. */
typedef enum Sd_ResultCode_
{
    SD_INIT         = 0x01U,    /*!< \brief initialized */
    SD_SEGMENT      = 0x02U,    /*!< \brief segment explored  */
    SD_CABLE_LINK   = 0x03U     /*!< \brief cable link diagnosis executed */

} Sd_ResultCode_t;

/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/

/*! \brief   Structure decribing a node of the segment to be tested. */
typedef struct Sd_Node_
{
    bool            available;          /*!< \brief node available? *//*! i todo RWI:  */
    uint16_t        node_address;       /*!< \brief node address used for welcome command */
    uint8_t         result;             /*!< \brief result parameter of Welcome.Result message */
    uint8_t         version;            /*!< \brief version parameter of Hello and Welcome messages */
    Ucs_Signature_t signature;          /*!< \brief signature of the node */

} Sd_Node;



/*! \brief   Structure of class CSysDiag. */
typedef struct CSysDiag_
{
    CInic   *inic;                      /*!< \brief Reference to CInic object */
    CExc    *exc;                       /*!< \brief Reference to CExc object */
    CBase   *base;                      /*!< \brief Reference to CBase object */

    bool     startup_locked;            /*!< \brief Locking of NetworkStartup without timeout */
    CSingleSubject sysdiag;             /*!< \brief Subject for the System Diagnosis reports */

    CSingleObserver sys_diag_start;     /*!< \brief Observes the Inic_NwSysDiagnosis() command */
    CSingleObserver sys_diag_stop;      /*!< \brief Observes the Inic_NwSysDiagEnd() command */
    CSingleObserver sys_hello;          /*!< \brief Observes the Hello  result */
    CSingleObserver sys_welcome;        /*!< \brief Observes the Welcome result */
    CSingleObserver sys_enable_port;    /*!< \brief Observes enabling a port */
    CSingleObserver sys_disable_port;   /*!< \brief Observes disabling a port */
    CSingleObserver sys_cable_link_diagnosis;   /*!< \brief Observes the CableLinkDiagnosis result */
    CMaskedObserver sys_terminate;      /*!< \brief  Observes events leading to termination */

    CFsm     fsm;                       /*!< \brief System Diagnosis state machine  */
    CService sd_srv;                    /*!< \brief Service instance for the scheduler */

    uint8_t  segment_nr;                /*!< \brief segment number which is currently checked*/
    uint8_t  num_ports;                 /*!< \brief number of ports of master node */
    uint8_t  curr_branch;               /*!< \brief branch which is currently examined */
    uint16_t admin_node_address;        /*!< \brief node address used during system diagnosis */
    Sd_ResultCode_t last_result;        /*!< \brief result of last segment
                                        */
    Sd_Node  master;                    /*!< \brief Timing Master node */
    Sd_Node  source;                    /*!< \brief Source node of segment to be tested  */
    Sd_Node  target;                    /*!< \brief Target node of segment to be tested  */
    uint16_t hello_retry;               /*!< \brief retry counter for hello message  */
    CTimer   timer;                     /*!< \brief timer for monitoring messages */

    Ucs_Sd_Report_t report;             /*!< \brief reports segment results */

} CSysDiag;


/*------------------------------------------------------------------------------------------------*/
/* Prototypes                                                                                     */
/*------------------------------------------------------------------------------------------------*/
extern void SysDiag_Ctor(CSysDiag *self, CInic *inic, CBase *base, CExc *exc);
extern Ucs_Return_t SysDiag_Run(CSysDiag *self, CSingleObserver *obs_ptr);
extern Ucs_Return_t SysDiag_Abort(CSysDiag *self);




#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* #ifndef UCS_SYS_DIAG_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

