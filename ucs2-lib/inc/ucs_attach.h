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
 * \brief Declaration of CAttachService class
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup  G_ATS
 * @{
 */

#ifndef UCS_ATTACH_H
#define UCS_ATTACH_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_obs.h"
#include "ucs_fsm.h"
#include "ucs_base.h"
#include "ucs_pmfifos.h"
#include "ucs_pmevent.h"
#include "ucs_inic.h"
#include "ucs_ret_pb.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Initialization structure of the attach service. */
typedef struct Ats_InitData_
{
    CBase *base_ptr;                    /*!< \brief Reference to UCS base instance */
    CPmFifos *fifos_ptr;                /*!< \brief Reference to PMS FIFOs */
    CPmEventHandler *pme_ptr;           /*!< \brief Reference to PMS Event Handler */
    CInic *inic_ptr;                    /*!< \brief Reference to INIC Management instance */

} Ats_InitData_t;

/*! \brief   Class structure of the attach service. */
typedef struct CAttachService_
{
    Ats_InitData_t init_data;               /*!< \brief Objects required for attach process */
    CFsm fsm;                               /*!< \brief Attach state machine */
    CService ats_srv;                       /*!< \brief Service instance for the scheduler */
    CObserver obs;                          /*!< \brief Observer used for asynchronous events */
    CObserver obs2;                         /*!< \brief Observer used for asynchronous events */
    CSingleObserver sobs;                   /*!< \brief Single-observer used for asynchronous
                                                        results */
    CMaskedObserver internal_error_obs;     /*!< \brief Error observer to handle INIC errors
                                                        during the attach process */
    CTimer timer;                           /*!< \brief Timer to check for init timeout */ 
    CSingleSubject ats_result_subject;      /*!< \brief Subject to report the result of the
                                                        attach process */
    Ucs_InitResult_t report_result;         /*!< \brief Internal result/error code memory */

} CAttachService;

/*------------------------------------------------------------------------------------------------*/
/* Prototypes of class CAttach                                                                    */
/*------------------------------------------------------------------------------------------------*/
extern void Ats_Ctor(CAttachService *self, Ats_InitData_t *init_ptr);
extern void Ats_Start(void *self, CSingleObserver *obs_ptr);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* #ifndef UCS_ATTACH_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

